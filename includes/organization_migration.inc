<?php
/**
 * @file
 * Organization migration.
 */

/**
 * Batch opp for migrating organizations.
 */
function utailla_migrate_organizations_batch_operation(&$context) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  $sandbox =& $context['sandbox'];
  if (!isset($sandbox['organizations'])) {
    $sandbox['progress'] = 0;
    $sandbox['organizations'] = utailla_get_organization_info();
    $sandbox['total'] = count($sandbox['organizations']);
  }
  $organization = array_shift($sandbox['organizations']);
  utailla_migrate_organization($organization);

  $sandbox['progress']++;
  $context['finished'] = $sandbox['progress'] / $sandbox['total'];
  $context['message'] = t('Processed @progress of @total.', array(
    '@progress' => $sandbox['progress'],
    '@total' => $sandbox['total'],
  ));
}

/**
 * Migrate an organization if it hasn't already been.
 */
function utailla_migrate_organization($org_info) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  module_load_include('inc', 'utailla', 'includes/utilities');
  if (!utailla_migrated_organization_pid($org_info->organization_id)) {
    $org = utailla_get_decoded_row_recursive($org_info);
    $tuque = islandora_get_tuque_connection();

    $object = $tuque->repository->constructObject('ailla');
    $object->models = array(UTAILLA_ORG_CMODEL);
    $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', UTAILLA_ORG_COLLECTION);
    $object->label = isset($org['fullname']) ? $org['fullname'] : 'Organization';

    $mads_ds = $object->constructDatastream('MADS', 'M');
    $mads_ds->label = 'MADS Record';
    $mads_ds->mimetype = 'application/xml';
    $mads_ds->content = utailla_build_organization_mads($org);
    $object->ingestDatastream($mads_ds);

    // Link to user.
    if ($org['user_id']) {
      $uid = utailla_get_migrated_user_uid($org['user_id']);
      if ($uid) {
        $object->relationships->add(
          ISLANDORA_RELS_EXT_URI,
          UTAILLA_USER_REL,
          utailla_get_uri_from_user($uid)
        );
      }
    }
    // Link to contact user.
    if ($org['contact_id']) {
      $uid = utailla_get_migrated_contact_uid($org['contact_id']);
      if ($uid) {
        $object->relationships->add(
          ISLANDORA_RELS_EXT_URI,
          UTAILLA_CONTACT_REL,
          utailla_get_uri_from_user($uid)
        );
      }
    }

    $tuque->repository->ingestObject($object);
    utailla_log_organization_mapping($org['organization_id'], $object->id);
    drupal_set_message(
      t(
        'Constructed an object for the organization @org with PID @pid.',
        array('@org' => $org['organization_id'], '@pid' => $object->id)
      )
    );
  }
  else {
    drupal_set_message(
      t(
        'An object already exists for the organization @org.',
        array('@org' => $org_info->organization_id)
      )
    );
  }
}

/**
 * Get content for an organization's MADS DS.
 */
function utailla_build_organization_mads($org) {
  module_load_include('inc', 'utailla', 'includes/utilities');
  $writer = utailla_get_mads_writer();

  // Organization name.
  if ($org['fullname']) {
    $writer->startElement('authority');
    $writer->startElement('name');
    $writer->writeAttribute('type', 'corporate');
    $writer->startElement('namePart');
    $writer->text($org['fullname']);
    $writer->endElement();
    $writer->endElement();
    $writer->endElement();
  }
  // Acronym.
  if ($org['acronym']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'acronym');
    $writer->text($org['acronym']);
    $writer->endElement();
  }
  // Director.
  if ($org['director']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'director');
    $writer->text($org['director']);
    $writer->endElement();
  }
  // Funder.
  if ($org['funder']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'funder');
    $writer->text($org['funder']);
    $writer->endElement();
  }
  // Parent institution.
  if ($org['parent_institution']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'parent institution');
    $writer->text($org['parent_institution']);
    $writer->endElement();
  }
  // English description.
  if ($org['description_en']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'biographical/historical');
    $writer->writeAttribute('lang', 'en');
    $writer->text($org['description_en']);
    $writer->endElement();
  }
  // Spanish description.
  if ($org['description_sp']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'biographical/historical');
    $writer->writeAttribute('lang', 'sp');
    $writer->text($org['description_sp']);
    $writer->endElement();
  }
  // Depositor status.
  if ($org['depositor']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'depositor status');
    $writer->text($org['depositor']);
    $writer->endElement();
  }
  // Languages.
  foreach ($org['languages'] as $language) {
    $pid = utailla_migrated_language_pid($language['language_id']);
    if ($pid) {
      $writer->startElement('fieldOfActivity');
      $writer->writeAttribute('type', 'research language');
      $writer->writeAttribute('authority', 'aillaLanguage');
      $writer->writeAttribute('valueURI', utailla_get_uri_from_pid($pid));
      $writer->endElement();
    }
  }

  $writer->endElement();
  $writer->endDocument();
  return $writer->flush();
}
