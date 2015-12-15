<?php
/**
 * @file
 * Collection migration functions.
 */

/**
 * Batch opp for migrating collections.
 */
function utailla_migrate_collections_batch_operation(&$context) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  $sandbox =& $context['sandbox'];
  if (!isset($sandbox['collections'])) {
    $sandbox['progress'] = 0;
    $sandbox['collections'] = utailla_get_collection_info();
    $sandbox['total'] = count($sandbox['collections']);
  }
  $collection = array_shift($sandbox['collections']);
  utailla_migrate_collection($collection);

  $sandbox['progress']++;
  $context['finished'] = $sandbox['progress'] / $sandbox['total'];
  $context['message'] = t('Processed @progress of @total.', array(
    '@progress' => $sandbox['progress'],
    '@total' => $sandbox['total'],
  ));
}

/**
 * Migrate a collection if it hasn't already been.
 */
function utailla_migrate_collection($collection_info) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  module_load_include('inc', 'utailla', 'includes/utilities');
  module_load_include('inc', 'utailla', 'includes/db');
  require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
  if (!utailla_migrated_collection_pid($collection_info->collection_id)) {
    $collection = utailla_get_decoded_row_recursive($collection_info);
    $tuque = islandora_get_tuque_connection();

    $object = $tuque->repository->constructObject('ailla');
    $object->models = array('islandora:collectionCModel');
    $object->label = isset($collection['titles'][0]['title']) ? $collection['titles'][0]['title'] : 'Collection';
    $tuque->repository->ingestObject($object);
    usleep(UTAILLA_USLEEP);
    // Add resource collection policy.
    utailla_set_collection_policy($object, 'resource_collection_policy.xml');
    usleep(UTAILLA_USLEEP);
    $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', UTAILLA_COLLECTION);
    usleep(UTAILLA_USLEEP);

    $mods_ds = $object->constructDatastream('MODS', 'M');
    $mods_ds->label = 'MODS Record';
    $mods_ds->mimetype = 'application/xml';
    $mods_ds->content = utailla_build_collection_mods($collection);
    $object->ingestDatastream($mods_ds);
    usleep(UTAILLA_USLEEP);

    if ($collection['notes']) {
      $note_ds = $object->constructDatastream('NOTE', 'M');
      $note_ds->label = 'Note';
      $note_ds->mimetype = 'application/xml';
      $note_ds->content = utailla_build_collection_notes($collection);
      $object->ingestDatastream($note_ds);
      utailla_restrict_datastream_to_admin($note_ds);
    }

    if ($collection['admin_notes']) {
      $admin_note_ds = $object->constructDatastream('ADMIN_NOTE', 'M');
      $admin_note_ds->label = 'Admin Note';
      $admin_note_ds->mimetype = 'application/xml';
      $admin_note_ds->content = utailla_build_collection_admin_notes($collection);
      $object->ingestDatastream($admin_note_ds);
      utailla_restrict_datastream_to_admin($admin_note_ds);
    }

    // Need to store collection restriction information for use later on.
    utailla_upsert_collection_restriction(array(
      'pid' => $object->id,
      'password' => empty($collection['password']) ? NULL : user_hash_password($collection['password']),
      'hint' => empty($collection['password_hint']) ? NULL : $collection['password_hint'],
    ));
    utailla_log_collection_mapping($collection['collection_id'], $object->id);
    drupal_set_message(
      t(
        'Constructed an object for the collection @collection with PID @pid.',
        array('@collection' => $collection['collection_id'], '@pid' => $object->id)
      )
    );
  }
  else {
    drupal_set_message(
      t(
        'An object already exists for the collection @collection.',
        array('@collection' => $collection_info->collection_id)
      )
    );
  }
}

/**
 * Get content for collection MODS datastream.
 */
function utailla_build_collection_mods($collection) {
  module_load_include('inc', 'utailla', 'includes/utilities');

  $writer = new XMLWriter();
  $writer->openMemory();
  $writer->startDocument('1.0', 'utf-8');
  $writer->startElement('mods');
  $writer->writeAttribute('xmlns', 'http://www.loc.gov/mods/v3');
  $writer->writeAttribute('xmlns:xlink', 'http://www.w3.org/1999/xlink');
  $writer->writeAttribute('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance');
  $writer->writeAttribute('xsi:schemaLocation', 'http://www.loc.gov/standards/mods/v3/mods-3-4.xsd');

  $writer->startElement('typeOfResource');
  $writer->writeAttribute('collection', 'yes');
  $writer->text('mixed material');
  $writer->endElement();

  // Website.
  if ($collection['website']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'source note');
    $writer->text($collection['website']);
    $writer->endElement();
  }

  // Titles.
  if ($collection['titles']) {
    foreach ($collection['titles'] as $title) {
      $writer->startElement('titleInfo');

      switch ($title['language']) {
        case 'ind':
          $writer->writeAttribute('displayLabel', 'Title (Indigenous language)');
          $writer->writeAttribute('type', 'alternative');
          break;

        case 'en':
          $writer->writeAttribute('displayLabel', 'Title (English)');
          $writer->writeAttribute('lang', 'eng');
          break;

        case 'sp':
          $writer->writeAttribute('displayLabel', 'Title (Spanish)');
          $writer->writeAttribute('lang', 'spa');
          break;

      }
      $writer->startElement('title');
      $writer->text($title['title']);
      $writer->endElement();
      $writer->endElement();
    }
  }

  // Languages.
  if ($collection['languages']) {
    foreach ($collection['languages'] as $language) {
      $pid = utailla_migrated_language_pid($language['language_id']);
      if ($pid) {
        $writer->startElement('language');
        $writer->startElement('languageTerm');
        $writer->writeAttribute('type', 'text');
        $writer->writeAttribute('authority', 'aillaLanguage');
        $writer->writeAttribute('valueURI', utailla_get_uri_from_pid($pid));
        $writer->endElement();
        $writer->endElement();
      }
    }
  }

  // Countries.
  if ($collection['countries']) {
    foreach ($collection['countries'] as $country) {
      $pid = utailla_migrated_country_pid($country['country_id']);
      if ($pid) {
        $writer->startElement('originInfo');
        $writer->writeAttribute('eventType', 'production');
        $writer->startElement('place');
        $writer->startElement('placeTerm');
        $writer->writeAttribute('type', 'text');
        $writer->writeAttribute('authority', 'aillaCountry');
        $writer->writeAttribute('valueURI', utailla_get_uri_from_pid($pid));
        $writer->endElement();
        $writer->endElement();
        $writer->endElement();
      }
    }
  }

  // Descriptions.
  if ($collection['descriptions']) {
    foreach ($collection['descriptions'] as $description) {
      $writer->startElement('abstract');
      switch ($description['language']) {
        case 'en':
          $writer->writeAttribute('displayLabel', 'Collection description (English)');
          $writer->writeAttribute('lang', 'eng');
          break;

        case 'sp':
          $writer->writeAttribute('displayLabel', 'Collection description (Spanish)');
          $writer->writeAttribute('lang', 'spa');
          break;

      }
      $writer->text($description['description']);
      $writer->endElement();
    }
  }

  // References.
  if ($collection['citations']) {
    $writer->startElement('relatedItem');
    $writer->writeAttribute('type', 'isReferencedBy');
    $writer->startElement('note');
    $writer->writeAttribute('type', 'citation/reference');
    $writer->text($collection['citations']);
    $writer->endElement();
    $writer->endElement();
  }

  $writer->endElement();
  $writer->endDocument();
  return $writer->flush();
}

/**
 * Get content for collection NOTE datastream.
 */
function utailla_build_collection_notes($collection) {
  $writer = new XMLWriter();
  $writer->openMemory();
  $writer->startDocument('1.0', 'utf-8');
  $writer->startElement('notes');

  $writer->startElement('note');
  $writer->text($collection['notes']);
  $writer->endElement();

  $writer->endElement();
  $writer->endDocument();
  return $writer->flush();
}

/**
 * Get content for collection ADMIN_NOTE datastream.
 */
function utailla_build_collection_admin_notes($collection) {
  $writer = new XMLWriter();
  $writer->openMemory();
  $writer->startDocument('1.0', 'utf-8');
  $writer->startElement('admin_notes');

  $writer->startElement('admin_note');
  $writer->text($collection['admin_notes']);
  $writer->endElement();

  $writer->endElement();
  $writer->endDocument();
  return $writer->flush();
}
