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
  $commands['utailla-migrate-all'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Run all migrations in the necessary order.',
    'drupal_dependencies' => array(
      'islandora',
    ),
    'examples' => array(
      'drush -u 1 -v utailla-migrate-all --directory=/home/vagrant/ailla_files',
    ),
    'options' => array(
      'directory' => array(
        'description' => dt("Directory to retrieve files from."),
      ),
    ),
  );
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

  $commands['utailla-migrate-countries'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'drupal_dependencies' => array(
      'islandora',
    ),
    'examples' => array(
      'drush -u 1 utailla-migrate-countries',
    ),
    'options' => array(
      'database' => array(
        'value' => 'optional',
        'description' => dt('The database name of where the AILLA database resides.'),
      ),
    ),
  );
  $commands['utailla-migrate-collections'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Migrate collections.',
    'drupal_dependencies' => array(
      'islandora',
    ),
    'examples' => array(
      'drush -u 1 -v utailla-migrate-collections',
    ),
  );
  $commands['utailla-migrate-organizations'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Migrate organizations.',
    'drupal_dependencies' => array(
      'islandora',
    ),
    'examples' => array(
      'drush -u 1 -v utailla-migrate-organizations',
    ),
  );
  $commands['utailla-migrate-resources'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Migrate resources.',
    'drupal_dependencies' => array(
      'islandora',
    ),
    'examples' => array(
      'drush -u 1 -v utailla-migrate-resources',
    ),
  );
  $commands['utailla-migrate-contributors'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Migrate contributors',
    'drupal_dependencies' => array('islandora'),
    'examples' => array('drush -u 1 -v utailla-migrate-contributors'),
  );
  $commands['utailla-media-batch-preprocess'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Preprocess media files for Islandora Batch.',
    'drupal_dependencies' => array('islandora', 'islandora_batch'),
    'examples' => array('drush -u 1 -v utailla-media-batch-preprocess --directory=/home/vagrant/ailla_files'),
    'options' => array(
      'directory' => array(
        'description' => dt("Directory to retrieve files from."),
      ),
    ),
  );
  $commands['utailla-dev-prep'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'DO NOT RUN ON PROD: IS DESTRUCTIVE.',
    'drupal_dependencies' => array('islandora'),
    'examples' => array('drush -u 1 -v utailla-dev-prep --directory=/home/vagrant/ailla_files --scp_user=dgi --password=PASS'),
    'options' => array(
      'directory' => array(
        'description' => dt("Directory to place scp'd files in."),
      ),
      'scp_user' => array(
        'description' => dt('The user for scp.'),
      ),
      'password' => array(
        'description' => dt('The password for scp.'),
      ),
    ),
  );
  return $commands;
}

/**
 * Drush command callback to preprocess media files for Islandora Batch.
 */
function drush_utailla_media_batch_preprocess() {
  $preprocessor = new UtaillaMediaBatch(
    islandora_get_tuque_connection(),
    array('directory' => drush_get_option('directory'), 'namespace' => 'ailla')
  );
  islandora_batch_handle_preprocessor($preprocessor);
}

/**
 * Drush command callback to prep for deving/testing the media migration.
 */
function drush_utailla_dev_prep() {
  module_load_include('inc', 'utailla', 'includes/dev_prep');
  utailla_dev_prep(drush_get_option('directory'));
}

/**
 * Drush command callback for running all migrations.
 */
function drush_utailla_migrate_all() {
  module_load_include('inc', 'utailla', 'includes/batch');
  $user_migration = Migration::getInstance(UTAILLA_USER_MIGRATION);
  $user_migration->processImport();
  $contact_migration = Migration::getInstance(UTAILLA_CONTACT_MIGRATION);
  $contact_migration->processImport();
  batch_set(utailla_migrate_languages_batch());
  batch_set(utailla_migrate_countries_batch());
  batch_set(utailla_migrate_collections_batch());
  batch_set(utailla_migrate_organizations_batch());
  batch_set(utailla_migrate_contributors_batch());
  batch_set(utailla_migrate_resources_batch());
  drush_backend_batch_process();
  $preprocessor = new UtaillaMediaBatch(
    islandora_get_tuque_connection(),
    array('directory' => drush_get_option('directory'), 'namespace' => 'ailla')
  );
  islandora_batch_handle_preprocessor($preprocessor);
}

/**
 * Drush command callback for migrating resources.
 */
function drush_utailla_migrate_resources() {
  module_load_include('inc', 'utailla', 'includes/batch');
  batch_set(utailla_migrate_resources_batch());
  drush_backend_batch_process();
}

/**
 * Drush command callback for migrating contributors.
 */
function drush_utailla_migrate_contributors() {
  module_load_include('inc', 'utailla', 'includes/batch');
  batch_set(utailla_migrate_contributors_batch());
  drush_backend_batch_process();
}

/**
 * Drush command callback for migrating organizations.
 */
function drush_utailla_migrate_organizations() {
  module_load_include('inc', 'utailla', 'includes/batch');
  batch_set(utailla_migrate_organizations_batch());
  drush_backend_batch_process();
}

/**
 * Drush command callback for migrating collections.
 */
function drush_utailla_migrate_collections() {
  module_load_include('inc', 'utailla', 'includes/batch');
  batch_set(utailla_migrate_collections_batch());
  drush_backend_batch_process();
}

/**
 * Migrates the languages from AILLA's old database.
 */
function drush_utailla_migrate_languages() {
  module_load_include('inc', 'utailla', 'includes/batch');
  batch_set(utailla_migrate_languages_batch(drush_get_option('database', 'for_migration')));
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
  module_load_include('inc', 'utailla', 'includes/migration.db');
  module_load_include('inc', 'utailla', 'includes/utilities');
  // Check to see if one already exists for the language id.
  if (!utailla_migrated_language_pid($row->language_id)) {
    // Create a new object for this language and add it to our mapping table.
    // Decode the rows for consistency.
    $row = utailla_get_decoded_row_recursive($row);
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
    $writer->text($row['name']);
    $writer->endElement();

    // Name (Alternative).
    $writer->startElement('aillaAltName');
    $writer->text($row['alternate_names']);
    $writer->endElement();

    // Language code.
    $writer->startElement('aillaLangCode');
    $writer->writeAttribute('authority', 'iso639-3');
    $writer->text($row['language_code']);
    $writer->endElement();

    // Language family indication.
    $writer->startElement('aillaLangFam');
    $writer->text($row['is_macro_code']);
    $writer->endElement();

    // Parent language.
    $writer->startElement('aillaParentLang');
    if (isset($all_rows[$row['parent_id']])) {
      $text_array = utailla_get_decoded_row_recursive(
        $all_rows[$row['parent_id']]->name
      );
      $writer->text($text_array[0]);
    }
    $writer->endElement();

    // Description (English).
    $writer->startElement('aillaEngDesc');
    $writer->text($row['description_en']);
    $writer->endElement();

    // Description (Spanish).
    $writer->startElement('aillaSpaDesc');
    $writer->text($row['description_sp']);
    $writer->endElement();

    $writer->endElement();
    $writer->endDocument();
    $ds->content = $writer->flush();
    $object->label = $row['name'];
    $object->ingestDatastream($ds);
    $tuque->repository->ingestObject($object);
    usleep(UTAILLA_USLEEP);
    db_insert('utailla_language_map')
      ->fields(array(
        'language_id' => $row['language_id'],
        'pid' => $object->id,
      ))
      ->execute();
    drush_log(t('Constructed an object for the language @lang with a pid of @pid.', array('@lang' => $row['language_code'], '@pid' => $object->id)));
  }
  else {
    drush_log(t('An object already exists with a language code of @lang.', array('@lang' => $row->language_code)));
  }
}

/**
 * Drush command callback to migrate countries from AILLA's database.
 */
function drush_utailla_migrate_countries() {
  module_load_include('inc', 'utailla', 'includes/batch');
  batch_set(utailla_migrate_countries_batch(drush_get_option('database', 'for_migration')));
  drush_backend_batch_process();
}

/**
 * Batch operation to migrate a country out of AILLA's database.
 */
function utailla_migrate_countries_batch_operation($database, &$context) {
  $sandbox =& $context['sandbox'];
  if (!isset($sandbox['rows'])) {
    $sandbox['progress'] = 0;
    $sandbox['rows'] = Database::getConnection('default', $database)
      ->select('country', 'c')
      ->fields('c', array())
      ->execute()
      ->fetchAllAssoc('country_id');
    // Stash the rows so we can re-use later without re-querying.
    $sandbox['total'] = count($sandbox['rows']);
    if ($sandbox['total'] == 0) {
      // Nothing to process.
      $context['finished'] = 1;

      $context['message'] = t('Nothing to migrate.');
      return;
    }
  }
  $row = array_shift($sandbox['rows']);
  utailla_migrate_country_construct_object($row);

  $sandbox['progress'] = min($sandbox['total'], $sandbox['progress'] + 1);
  $context['finished'] = $sandbox['progress'] / $sandbox['total'];
  $context['message'] = t('Processed @progress of @total.', array(
    '@progress' => $sandbox['progress'],
    '@total' => $sandbox['total'],
  ));
}

/**
 * Construct an object if one doesn't exist already for the row.
 */
function utailla_migrate_country_construct_object($row) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  module_load_include('inc', 'utailla', 'includes/utilities');
  // Check to see if one already exists for the language id.
  if (!utailla_migrated_country_pid($row->country_id)) {
    // Create a new object for this country and add it to our mapping table.
    $tuque = islandora_get_tuque_connection();
    $object = $tuque->repository->constructObject('ailla');
    $object->models = array('ailla:countryCModel');
    $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', 'ailla:country_collection');

    $ds = $object->constructDatastream('MADS', 'M');
    $ds->label = 'MADS Record';
    $ds->mimetype = 'application/xml';

    $writer = utailla_get_mads_writer();

    // Country name.
    $writer->startElement('authority');
    $writer->startElement('geographic');
    $writer->writeAttribute('lang', 'eng');
    $writer->writeAttribute('authority', 'iso3166-1');
    $writer->text($row->country_name);
    $writer->endElement();

    // Country code.
    $writer->startElement('geographic');
    $writer->writeAttribute('authority', 'iso3166-1');
    $writer->text($row->country_code);
    $writer->endElement();

    $writer->endElement();
    $writer->endDocument();
    $ds->content = $writer->flush();
    $object->label = $row->country_name;
    $object->ingestDatastream($ds);
    $tuque->repository->ingestObject($object);
    usleep(UTAILLA_USLEEP);
    db_insert('utailla_country_map')
      ->fields(array(
        'country_id' => $row->country_id,
        'pid' => $object->id,
      ))
      ->execute();
    drush_log(t('Constructed an object for the country @code with a pid of @pid.', array('@code' => $row->country_code, '@pid' => $object->id)));
  }
  else {
    drush_log(t('An object already exists with a country code of @code.', array('@code' => $row->country_code)));
  }
}
