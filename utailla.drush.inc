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
  $commands['utailla-update-resource-policy'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Update Resource Content Policies',
    'drupal_dependencies' => array(
      'islandora',
    ),
    'examples' => array(
      'drush -u 1 -v utailla-update-resource-policy',
    ),
  );
  $commands['utailla-clean-text-extents'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Wipe time-like extents from text objects.',
    'drupal dependencies' => array(
      'islandora',
      'islandora_solr',
    ),
  );
  $commands['utailla-clean-multimedia-extents'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    'description' => 'Wipe page-like extents from multimedia (video and audio) objects.',
    'drupal dependencies' => array(
      'islandora',
      'islandora_solr',
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
  batch_set(utailla_migrate_organizations_batch());
  batch_set(utailla_migrate_contributors_batch());
  batch_set(utailla_migrate_collections_batch());
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
  $chunk_size = 10;
  foreach (array_slice($sandbox['rows'], 0, $chunk_size) as $row) {
    utailla_migrate_language_construct_object($row, $sandbox['stashed_rows']);
    $sandbox['progress'] = min($sandbox['total'], $sandbox['progress'] + 1);
  }
  $sandbox['rows'] = array_slice($sandbox['rows'], $chunk_size);

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
    $alternate_names = explode(';', $row['alternate_names']);
    foreach ($alternate_names as $name) {
      $writer->startElement('aillaAltName');
      $writer->text(trim($name));
      $writer->endElement();
    }

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

/**
 * Drush command callback to update the Collection Policy of Resource objects.
 */
function drush_utailla_update_resource_policy() {
  $batch = array(
    'file' => drupal_get_path('module', 'utailla') . '/utailla.drush.inc',
    'operations' => array(
      array(
        'utailla_resource_policy_batch_operation',
        array(),
      ),
    ),
  );
  batch_set($batch);
  drush_backend_batch_process();
}

/**
 * Batch operation to update Resource Collection Policy datastreams.
 *
 * @param $context
 */
function utailla_resource_policy_batch_operation(&$context) {
  $sandbox =& $context['sandbox'];
  if (!isset($sandbox['pids'])) {
    $sandbox['progress'] = 0;
    $sandbox['pids'] = utailla_drush_cmodel_query_pids(UTAILLA_RESOURCE_CMODEL);
    $sandbox['total'] = count($sandbox['pids']);
    if ($sandbox['total'] == 0) {
      // Nothing to process.
      $context['finished'] = 1;
      $context['message'] = t('Nothing to update.');
      return;
    }
    else {
      $context['finished'] = 0;
    }
  }

  $local_count = 0;
  while ($local_count < 40 && $sandbox['progress'] < $sandbox['total']) {
    $pid = array_shift($sandbox['pids']);

    $object = islandora_object_load($pid);

    $collection_policy_path = drupal_get_path('module', 'utailla') . '/xml/media_collection_policy.xml';
    $object['COLLECTION_POLICY']->setContentFromFile($collection_policy_path);

    $sandbox['progress'] = min($sandbox['total'], $sandbox['progress'] + 1);
    $context['finished'] = $sandbox['progress'] / $sandbox['total'];
    $local_count++;
  }

  $context['message'] = t('Processed @progress of @total.', array(
    '@progress' => $sandbox['progress'],
    '@total' => $sandbox['total'],
  ));
}

/**
 * Query helper to get a list of PIDs having the defined content model.
 *
 * @param $content_model
 *   Content Model of the objects to query.
 *
 * @return array
 */
function utailla_drush_cmodel_query_pids($content_model) {
  $query = <<<EOQ
SELECT ?pid
FROM <#ri>
WHERE {
  ?pid <fedora-model:hasModel> <info:fedora/$content_model>
}
EOQ;
// Connect to Tuque and grab the results.
  $connection = islandora_get_tuque_connection();
  $results = $connection->repository->ri->sparqlQuery($query);

  $pids = array();
  foreach ($results as $result) {
    $pids[] = $result['pid']['value'];
  }

  return $pids;
}

/**
 * Command callback; Queue up and kick off the batch.
 */
function drush_utailla_clean_text_extents() {
  batch_set(array(
    'operations' => array(
      array(
        'utailla_clean_extents_batch_op',
        array(
          array(
            'islandora:sp_pdf',
            'islandora:sp_document',
            'islandora:binaryObjectCModel',
          ),
          array(
            format_string('!field:"0:0:0"', array(
              '!field' => 'mods_physicalDescription_extent_ms',
            )),
          ),
          'utailla_nuke_elements',
          array(
            '//mods:mods[mods:typeOfResource/text() = "text"]/mods:physicalDescription/mods:extent[text() = "0:0:0"]',
          ),
        ),
      ),
    ),
  ));
  drush_backend_batch_process();
}

/**
 * Batch operation; Clean text extents of time-like content.
 */
function utailla_clean_extents_batch_op($models, $fqs, $callback, $callback_args, &$context) {
  $sandbox = &$context['sandbox'];

  $created_field = 'fgs_createdDate_dt';

  if (!isset($sandbox['to_process']) || empty($sandbox['to_process'])) {
    $add_uri_prefix_and_quote = function ($model) {
      return format_string('"info:fedora/!model"', array(
        '!model' => $model,
      ));
    };
    $map_to_process = function ($result) use ($created_field) {
      $doc = $result['solr_doc'];
      return array($doc['PID'], $doc[$created_field]);
    };

    // Query for some PIDs to clean up.
    $qp = new IslandoraSolrQueryProcessor();
    $qp->buildQuery('*:*');
    $qp->solrParams['fl'] = "PID,$created_field";
    $qp->solrParams['fq'] = $fqs;
    $qp->solrParams['fq'][] = format_string('!field:(!values)', array(
      '!field' => 'RELS_EXT_hasModel_uri_ms',
      '!values' => implode(' OR ', array_map($add_uri_prefix_and_quote, $models)),
    ));
    if (isset($sandbox['last_processed']) && $sandbox['last_processed']) {
      $qp->solrParams['fq'][] = format_string('!date_field:{"!start_date" TO *} OR (!date_field:"!start_date" AND PID:{"!start_pid" TO *})', array(
        '!date_field' => $created_field,
        '!start_date' => $sandbox['last_processed'][1],
        '!start_pid' => $sandbox['last_processed'][0],
      ));
    }
    $qp->solrParams['sort'] = "$created_field asc, PID asc";
    $qp->solrLimit = 1000;
    $qp->executeQuery(FALSE, TRUE);
    $sandbox['to_process'] = array_map($map_to_process, $qp->islandoraSolrResult['response']['objects']);
  }

  if (empty($sandbox['to_process'])) {
    // Done.
    return;
  }

  // Cleanup the MODS for a given object.
  list($pid, $date) = $sandbox['last_processed'] = array_shift($sandbox['to_process']);
  $object = islandora_object_load($pid);
  $count = call_user_func_array($callback, array_merge(array($object), $callback_args));

  $context['message'] = format_plural($count, 'Processed @pid, changing 1 occurence.', 'Processed @pid, changing @count occurences.', array(
    '@pid' => $pid,
  ));
  $context['finished'] = 0;
}

/**
 * Helper function; remove time-like extent entries from text.
 */
function utailla_nuke_elements(AbstractObject $object, $path) {
  if (!isset($object['MODS'])) {
    throw new InvalidArgumentException(t('The given object does not appear to have a MODS datastream: @pid', array(
      '@pid' => $object->id,
    )));
  }

  $doc = new DOMDocument();
  $doc->loadXML($object['MODS']->content);
  $xpath = new DOMXPath($doc);
  $xpath->registerNamespace('mods', 'http://www.loc.gov/mods/v3');
  $results = $xpath->query($path);
  foreach ($results as $result) {
    $result->parentNode->removeChild($result);
  }
  $length = $results->length;
  if ($length > 0) {
    $object['MODS']->content = $doc->saveXML();
  }
  return $length;
}

/**
 * Command callback; Queue up and kick off the batch.
 */
function drush_utailla_clean_multimedia_extents() {
  batch_set(array(
    'operations' => array(
      array(
        'utailla_clean_extents_batch_op',
        array(
          array(
            'islandora:sp_videoCModel',
            'islandora:sp-audioCModel',
          ),
          array(
            format_string('!field:"0"', array(
              '!field' => 'mods_physicalDescription_extent_ms',
            )),
          ),
          'utailla_nuke_elements',
          array(
            '//mods:mods/mods:physicalDescription/mods:extent[@displayLabel="Length in pages" and text() = "0"]',
          ),
        ),
      ),
    ),
  ));
  drush_backend_batch_process();
}
