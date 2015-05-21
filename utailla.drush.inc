<?php
/**
 * @file
 * Drush hook implementations for UTAILLA.
 */

/**
 * Implements hook_drush_command().
 */
function utailla_drush_command() {
  $commands = array();

  $commands['utailla-migrate-languages'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'drupal_dependencies' => array(
      'islandora',
    ),
    'examples' => array(
      'drush -u 1 utailla-migrate-languages',
    ),
    'options' => array(
      'database' => array(
        'value' => 'optional',
        'description' => dt('The database name of where the AILLA database resides.'),
      ),
    ),
  );
  return $commands;
}

/**
 * Migrates the languages from AILLA's old database.
 */
function drush_utailla_migrate_languages() {
  $batch = array(
    'operations' => array(
      array(
        'utailla_migrate_language_batch_operation',
        array(
          drush_get_option('database', 'for_migration'),
        ),
      ),
    ),
  );
  batch_set($batch);
  drush_backend_batch_process();
}

/**
 * Batch operation to migrate a language out of AILLA's database.
 */
function utailla_migrate_language_batch_operation($database, &$context) {
  $sandbox =& $context['sandbox'];
  if (!isset($sandbox['rows'])) {
    $sandbox['progress'] = 0;
    $sandbox['rows'] = Database::getConnection('default', $database)
      ->select('language', 'l')
      ->fields('l', array())
      ->execute()
      ->fetchAllAssoc('language_id');
    // Stash the rows so we can re-use later without re-querying.
    $sandbox['stashed_rows'] = $sandbox['rows'];
    $sandbox['total'] = count($sandbox['rows']);
    if ($sandbox['total'] == 0) {
      // Nothing to process.
      $context['finished'] = 1;

      $context['message'] = t('Nothing to fix.');
      return;
    }
  }
  $row = array_shift($sandbox['rows']);
  utailla_migrate_language_construct_object($row, $sandbox['stashed_rows']);

  $sandbox['progress'] = min($sandbox['total'], $sandbox['progress'] + 1);
  $context['finished'] = $sandbox['progress'] / $sandbox['total'];
  $context['message'] = t('Processed @progress of @total.', array(
    '@progress' => $sandbox['progress'],
    '@total' => $sandbox['total'],
  ));
}

/**
 * Construct an object if one doesn't already exist for the row.
 */
function utailla_migrate_language_construct_object($row, $all_rows) {
  // Check to see if one already exists for the language id.
  $exists = db_select('utailla_language_map', 'u')
    ->condition('language_id', $row->language_id)
    ->fields('u', array('language_id'))
    ->execute()
    ->rowCount();
  if (!$exists) {
    // Create a new object for this language and add it to our mapping table.
    // Decode the rows for consistency.
    foreach ($row as &$value) {
      $value = html_entity_decode($value, ENT_QUOTES, 'UTF-8');
    }
    unset($value);
    $tuque = islandora_get_tuque_connection();
    $object = $tuque->repository->constructObject('ailla');
    $object->models = array('ailla:languageCModel');
    $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', 'ailla:language_collection');

    $ds = $object->constructDatastream('LANGUAGE', 'M');
    $ds->label = 'Language Mapping';
    $ds->mimetype = 'application/xml';

    // Construct the XML from the passed in row.
    $writer = new XMLWriter();
    $writer->openMemory();
    $writer->startDocument('1.0', 'utf-8');
    $writer->startElement('aillaLanguage');

    // Name (English).
    $writer->startElement('aillaEngName');
    $writer->writeAttribute('authority', 'iso639-3');
    $writer->text($row->name);
    $writer->endElement();

    // Name (Alternative).
    $writer->startElement('aillaAltName');
    $writer->text($row->alternate_names);
    $writer->endElement();

    // Language code.
    $writer->startElement('aillaLangCode');
    $writer->writeAttribute('authority', 'iso639-3');
    $writer->text($row->language_code);
    $writer->endElement();

    // Language family indication.
    $writer->startElement('aillaLangFam');
    $writer->text($row->is_macro_code);
    $writer->endElement();

    // Parent language.
    $writer->startElement('aillaParentLang');
    if (isset($all_rows[$row->parent_id])) {
      $writer->text(html_entity_decode($all_rows[$row->parent_id]->name));
    }
    $writer->endElement();

    // Description (English).
    $writer->startElement('aillaEngDesc');
    $writer->text($row->description_en);
    $writer->endElement();

    // Description (Spanish).
    $writer->startElement('aillaSpaDesc');
    $writer->text($row->description_sp);
    $writer->endElement();

    $writer->endElement();
    $writer->endDocument();
    $ds->content = $writer->flush();
    $object->label = $row->name;
    $object->ingestDatastream($ds);
    $tuque->repository->ingestObject($object);
    db_insert('utailla_language_map')
      ->fields(array(
        'language_id' => $row->language_id,
        'pid' => $object->id,
      ))
      ->execute();
    drush_log(t('Constructed an object for the language @lang with a pid of @pid.', array('@lang' => $row->language_code, '@pid' => $object->id)));
  }
  else {
    drush_log(t('An object already exists with a language code of @lang.', array('@lang' => $row->language_code)));
  }
}
