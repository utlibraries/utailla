<?php
/**
 * @file
 * Contributor migration.
 */

/**
 * Contributor migration batch opp.
 */
function utailla_migrate_contributors_batch_operation(&$context) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  $sandbox =& $context['sandbox'];
  $step = 100;
  if (!isset($sandbox['total'])) {
    $sandbox['progress'] = 0;
    $sandbox['total'] = utailla_get_contributor_count();
  }

  $items = utailla_get_contributor_info($sandbox['progress'], $step);
  foreach ($items as $item) {
    utailla_migrate_contributor($item);
    $sandbox['progress']++;
  }

  $context['finished'] = $sandbox['progress'] / $sandbox['total'];
  $context['message'] = t('Processed @progress of @total.', array(
    '@progress' => $sandbox['progress'],
    '@total' => $sandbox['total'],
  ));
}

/**
 * Migrate a contributor if it has not already been.
 */
function utailla_migrate_contributor($contributor_info) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  module_load_include('inc', 'utailla', 'includes/utilities');
  if (!utailla_migrated_contributor_pid($contributor_info->contributor_id)) {
    $contributor = utailla_get_decoded_row_recursive($contributor_info);
    $tuque = islandora_get_tuque_connection();

    $object = $tuque->repository->constructObject('ailla');
    $object->models = array(UTAILLA_CONTRIBUTOR_CMODEL);
    $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', UTAILLA_CONTRIBUTOR_COLLECTION);
    $object->label = utailla_concat_name_parts($contributor);
    utailla_set_user_rels($object, $contributor);

    $mads_ds = $object->constructDatastream('MADS', 'M');
    $mads_ds->label = 'MADS Record';
    $mads_ds->mimetype = 'application/xml';
    $mads_ds->content = utailla_build_contributor_mads($contributor);
    $object->ingestDatastream($mads_ds);

    $tuque->repository->ingestObject($object);
    usleep(UTAILLA_USLEEP);
    utailla_log_contributor_mapping($contributor['contributor_id'], $object->id);
    drupal_set_message(
      t(
        'Constructed an object for the contributor @item with PID @pid.',
        array('@item' => $contributor['contributor_id'], '@pid' => $object->id)
      )
    );
  }
  else {
    drupal_set_message(
      t(
        'An object already exists for the contributor @item.',
        array('@item' => $contributor_info->contributor_id)
      )
    );
  }
}

/**
 * Get content for a contributor's MADS DS.
 */
function utailla_build_contributor_mads($contributor) {
  $writer = utailla_get_mads_writer();
  utailla_write_common_mads($writer, $contributor);

  if ($contributor['first_name'] || $contributor['last_name']) {
    // Open the authority record.
    $writer->startElement('authority');
    $writer->startElement('name');
    // First name.
    if ($contributor['first_name']) {
      $writer->writeAttribute('type', 'personal');
      $writer->startElement('namePart');
      $writer->writeAttribute('type', 'given');
      $writer->text($contributor['first_name']);
      $writer->endElement();
    }
    // Last name.
    if ($contributor['last_name']) {
      $writer->writeAttribute('type', 'personal');
      $writer->startElement('namePart');
      $writer->writeAttribute('type', 'family');
      $writer->text($contributor['last_name']);
      $writer->endElement();
    }
    $writer->endElement();
    $writer->endElement();
  }

  // Sex.
  if ($contributor['sex']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'biographical/historical');
    $writer->text($contributor['sex']);
    $writer->endElement();
  }

  // Birth date.
  if ($contributor['birth_date'] && $contributor['birth_date'] != '0000-00-00') {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'biographical/historical');
    $writer->text($contributor['birth_date']);
    $writer->endElement();
  }

  // Keep anonymous.
  if ($contributor['keep_anonymous']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'anonymous');
    $writer->text($contributor['keep_anonymous']);
    $writer->endElement();
  }

  // Nickname.
  if ($contributor['nickname']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'nickname');
    $writer->text($contributor['nickname']);
    $writer->endElement();
  }

  // Origin.
  if ($contributor['origin']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'biographical/historical');
    $writer->text($contributor['origin']);
    $writer->endElement();
  }

  // Languages.
  foreach ($contributor['languages'] as $language) {
    $language_type_found = FALSE;
    if ($language['native_language'] == 'Y') {
      $language_type_found = TRUE;
      utailla_write_mads_language($writer, $language['language_id'], 'native language');
    }
    if ($language['research_language'] == 'Y') {
      $language_type_found = TRUE;
      utailla_write_mads_language($writer, $language['language_id']);
    }
    if (!$language_type_found) {
      utailla_write_mads_language($writer, $language['language_id'], 'other language');
    }
  }
  // Organizations.
  if (!empty($contributor['organizations'])) {
    $writer->startElement('affiliation');
    foreach ($contributor['organizations'] as $org) {
      $pid = utailla_migrated_organization_pid($org['organization_id']);
      if ($pid) {
        $writer->startElement('organization');
        $writer->writeAttribute('authority', 'aillaOrganization');
        $writer->writeAttribute('valueURI', utailla_get_uri_from_pid($pid));
        $writer->endElement();
      }
    }
    $writer->endElement();
  }

  $writer->endElement();
  $writer->endDocument();
  return $writer->flush();
}
