<?php
/**
 * @file
 * Resource migration.
 */

/**
 * Batch opp for migrating resources.
 */
function utailla_migrate_resources_batch_operation(&$context) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  $sandbox =& $context['sandbox'];
  if (!isset($sandbox['resources'])) {
    $sandbox['progress'] = 0;
    $sandbox['resources'] = utailla_get_resource_info();
    $sandbox['total'] = count($sandbox['resources']);
  }
  $resource = array_shift($sandbox['resources']);
  utailla_migrate_resource($resource);

  $sandbox['progress']++;
  $context['finished'] = $sandbox['progress'] / $sandbox['total'];
  $context['message'] = t('Processed @progress of @total.', array(
    '@progress' => $sandbox['progress'],
    '@total' => $sandbox['total'],
  ));
}

/**
 * Migrate an resource if it hasn't already been.
 */
function utailla_migrate_resource($resource_info) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  module_load_include('inc', 'utailla', 'includes/utilities');
  if (!utailla_migrated_resource_pid($resource_info->resource_id)) {
    $resource = utailla_get_decoded_row_recursive($resource_info);
    $tuque = islandora_get_tuque_connection();

    $object = $tuque->repository->constructObject('ailla');
    $object->models = array(UTAILLA_RESOURCE_CMODEL);
    // Add all collection relationships.
    if ($resource['collections']) {
      foreach ($resource['collections'] as $collection) {
        $pid = utailla_migrated_collection_pid($collection['collection_id']);
        if ($pid) {
          $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $pid);
        }
      }
    }
    // Note: some of the data being migrated has empty title_text.
    $english = $spanish = $other = NULL;
    if ($resource['titles']) {
      foreach ($resource['titles'] as $title) {
        switch ($title['language']) {
          case 'other':
            $other = $title['title_text'];
            break;

          case 'en':
            $english = $title['title_text'];
            break;

          case 'sp':
            $spanish = $title['title_text'];
            break;
        }
      }
    }

    // Title order: english > spanish > other > "Resource".
    if (!empty($english)) {
      $object->label = $english;
    }
    elseif (!empty($spanish)) {
      $object->label = $spanish;
    }
    elseif (!empty($other)) {
      $object->label = $other;
    }
    else {
      $object->label = 'Resource';
    }

    $mads_ds = $object->constructDatastream('MADS', 'M');
    $mads_ds->label = 'MADS Record';
    $mads_ds->mimetype = 'application/xml';
    $mads_ds->content = utailla_build_resource_mads($resource);
    $object->ingestDatastream($mads_ds);

    $tuque->repository->ingestObject($object);
    utailla_log_resource_mapping($resource['resource_id'], $object->id);
    drupal_set_message(
      t(
        'Constructed an object for the resource @resource with PID @pid.',
        array('@resource' => $resource['resource_id'], '@pid' => $object->id)
      )
    );
  }
  else {
    drupal_set_message(
      t(
        'An object already exists for the resource @resource.',
        array('@resource' => $resource_info->resource_id)
      )
    );
  }
}

/**
 * Get content for an resource's MADS DS.
 */
function utailla_build_resource_mads($resource) {
  module_load_include('inc', 'utailla', 'includes/utilities');
  $writer = utailla_get_mads_writer();

  // Titles.
  if ($resource['titles']) {
    foreach ($resource['titles'] as $title) {
      if (!empty($title['title_text'])) {
        $writer->startElement('titleInfo');

        switch ($title['language']) {
          case 'other':
            $writer->writeAttribute('displayLabel', 'Indigenous Title');
            $writer->writeAttribute('type', 'alternative');
            break;

          case 'en':
            $writer->writeAttribute('lang', 'eng');
            break;

          case 'sp':
            $writer->writeAttribute('lang', 'spa');
            break;

        }
        $writer->startElement('title');
        $writer->text($title['title_text']);
        $writer->endElement();
        $writer->endElement();
      }
    }
  }

  // Contributors and role.
  foreach ($resource['contributors'] as $contributor) {
    $pid = utailla_migrated_contributor_pid($contributor['contributor_id']);
    $object = islandora_object_load($pid);
    if ($object) {
      $writer->startElement('name');
      $writer->writeAttribute('type', 'personal');
      $writer->writeAttribute('type', 'corporate');
      $writer->startElement('namePart');
      // Contributor.
      $writer->text($object->label);
      // Role in english.
      $writer->startElement('role');
      $writer->startElement('roleTerm');
      $writer->writeAttribute('type', 'text');
      $writer->writeAttribute('lang', 'eng');
      $writer->text($contributor['role_name']);
      // Role in spanish.
      $writer->startElement('role');
      $writer->startElement('roleTerm');
      $writer->writeAttribute('type', 'text');
      $writer->writeAttribute('lang', 'eng');
      $writer->text($contributor['papel_nombre']);
      $writer->endElement();
      $writer->endElement();
      $writer->endElement();
      $writer->endElement();
    }
  }

  // Languages.
  foreach ($resource['languages'] as $language) {
    $pid = utailla_migrated_language_pid($language['language_id']);
    if ($pid) {
      $writer->startElement('language');
      $writer->startElement('languageTerm');
      $writer->writeAttribute('type', 'text');
      $writer->writeAttribute('authority', 'aillaLanguage');
      $writer->writeAttribute('valueURI', utailla_get_uri_from_pid($pid));
      $writer->endElement();
    }
  }

  // Country.
  if ($resource['country_id']) {
    $pid = utailla_migrated_country_pid($resource['country_id']);
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

  // Descriptions.
  if ($resource['descriptions']) {
    foreach ($resource['descriptions'] as $description) {
      $writer->startElement('abstract');
      switch ($description['language']) {
        case 'en':
          $writer->writeAttribute('lang', 'eng');
          break;

        case 'sp':
          $writer->writeAttribute('lang', 'spa');
          break;

      }
      $writer->writeAttribute('displayLabel', 'Collection Description');
      $writer->text($description['description']);
      $writer->endElement();
    }
  }

  // Legacy Resource ID.
  if ($resource['identifier']) {
    $writer->startElement('identifier');
    $writer->writeAttribute('type', 'local');
    $writer->writeAttribute('displayLabel', 'Legacy Resource ID');
    $writer->text($resource['identifier']);
    $writer->endElement();
  }

  // Collection Name.
  if ($resource['collections']) {
    foreach ($resource['collections'] as $collection) {
      $pid = utailla_migrated_collection_pid($collection['collection_id']);
      $collection = islandora_object_load($pid);
      if ($collection) {
        $writer->startElement('relatedItem');
        $writer->writeAttribute('type', 'host');
        $writer->startElement('titleInfo');
        $writer->writeAttribute('displayLabel', 'Collection name');
        $writer->startElement('title');
        $writer->text($collection->label);
        $writer->endElement();
        $writer->endElement();
        $writer->endElement();
      }
    }
  }

  // Language Community.
  if ($resource['language_community']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'language community');
    $writer->text($resource['language_community']);
    $writer->endElement();
  }

  // Source Note.
  if ($resource['original_identifier']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'source note');
    $writer->text($resource['original_identifier']);
    $writer->endElement();
  }

  // Place.
  if ($resource['place']) {
    $writer->startElement('originInfo');
    $writer->writeAttribute('eventType', 'production');
    $writer->startElement('place');
    $writer->startElement('placeTerm');
    $writer->writeAttribute('type', 'text');
    $writer->text($resource['place']);
    $writer->endElement();
    $writer->endElement();
    $writer->endElement();
  }

  // Date Created.
  if ($resource['date_created']) {
    $writer->startElement('originInfo');
    $writer->writeAttribute('eventType', 'production');
    $writer->startElement('dateCreated');
    $writer->text($resource['date_created']);
    $writer->endElement();
    $writer->endElement();
  }

  // Type of Resource.
  $writer->startElement('typeOfResource');
  $writer->writeAttribute('collection', 'yes');
  $writer->text('mixed material');
  $writer->endElement();

  // Genres.
  if ($resource['genres']) {
    foreach ($resource['genres'] as $genre) {
      $writer->startElement('genre');
      $writer->writeAttribute('lang', 'eng');
      $writer->writeAttribute('type', 'local');
      $writer->text($genre['name']);
      $writer->endElement();

      $writer->startElement('genre');
      $writer->writeAttribute('lang', 'spa');
      $writer->writeAttribute('type', 'local');
      $writer->text($genre['nombre']);
      $writer->endElement();
    }
  }

  // Reference/Citation.
  if ($resource['citations']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'citation/reference');
    $writer->text($resource['citations']);
    $writer->endElement();
  }

  $writer->endElement();
  $writer->endDocument();
  return $writer->flush();
}
