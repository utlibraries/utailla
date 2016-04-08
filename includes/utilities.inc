<?php
/**
 * @file
 * Helper functions.
 */

/**
 * Ingests the default thumbnail on an object.
 */
function utailla_set_default_thumb(AbstractObject $object) {
  $islandora_path = drupal_get_path('module', 'islandora');
  $datastream = $object->constructDatastream('TN', 'M');
  $datastream->label = 'Thumbnail';
  $datastream->mimetype = 'image/png';
  $datastream->setContentFromFile("$islandora_path/images/folder.png", FALSE);
  $object->ingestDatastream($datastream);
}

/**
 * Ingests the collection policy.
 */
function utailla_set_collection_policy(AbstractObject $object, $file_name) {
  $module_path = drupal_get_path('module', 'utailla');
  $collection_policy = $object->constructDatastream('COLLECTION_POLICY', 'M');
  $collection_policy->label = 'Collection Policy';
  $collection_policy->mimetype = 'application/xml';
  $collection_policy->setContentFromFile("$module_path/xml/$file_name", FALSE);
  $object->ingestDatastream($collection_policy);
}

/**
 * Ingests the DS-COMPOSITE.
 */
function utailla_set_ds_composite(AbstractObject $object, $file_name) {
  $module_path = drupal_get_path('module', 'utailla');
  $ds_composite = $object->constructDatastream('DS-COMPOSITE-MODEL', 'M');
  $ds_composite->label = 'DS-COMPOSITE-MODEL';
  $ds_composite->mimetype = 'application/xml';
  $ds_composite->setContentFromFile("$module_path/xml/$file_name", FALSE);
  $object->ingestDatastream($ds_composite);
}

/**
 * Recursively HTML decode and replace tags on a database row.
 *
 * The originating database had HTML fragments that we transform into text the
 * only tag was br so we are replacing these with newlines.
 *
 * @param array $row_object
 *   Recursive database array.
 * @param array $ignore_brs
 *   List of indexes not to remove the br from.
 */
function utailla_get_decoded_row_recursive($row_object, $ignore_brs = array()) {
  $row_array = (array) $row_object;
  foreach ($row_array as $index => $column) {
    if (is_array($column)) {
      foreach ($column as $key => $value) {
        $row_array[$index][$key] = utailla_get_decoded_row_recursive($value);
      }
    }
    else {
      module_load_include('inc', 'islandora', 'includes/utilities');
      $row_array[$index] = trim($column);
      // Remove all invalid XML characters from string.
      $row_array[$index] = islandora_sanitize_input_for_valid_xml($row_array[$index]);
      $row_array[$index] = html_entity_decode($row_array[$index], ENT_QUOTES, 'UTF-8');
      if (!in_array($index, $ignore_brs)) {
        $row_array[$index] = preg_replace(
          '/<br\s*\/?>/i',
          "\n",
          $row_array[$index]
        );
      }
    }
  }
  return $row_array;
}

/**
 * Restrict a datastream to the admin role.
 */
function utailla_restrict_datastream_to_admin($ds) {
  $xacml = new IslandoraXacml($ds->parent);
  $xacml->datastreamRule->addDsid($ds->id);
  $xacml->datastreamRule->addRole('admin');
  $xacml->writeBackToFedora();
}

/**
 * Get a site specific URI from a PID.
 */
function utailla_get_uri_from_pid($pid) {
  global $base_url;
  return "$base_url/islandora/object/$pid";
}

/**
 * Get a site specific URI from a user.
 */
function utailla_get_uri_from_user($uid) {
  global $base_url;
  return "$base_url/user/$uid";
}

/**
 * Get a MADS xml writer.
 */
function utailla_get_mads_writer() {
  $writer = new XMLWriter();
  $writer->openMemory();
  $writer->startDocument('1.0', 'utf-8');
  $writer->startElement('mads');
  $writer->writeAttribute('xmlns', 'http://www.loc.gov/mads/v2');
  $writer->writeAttribute('xmlns:xlink', 'http://www.w3.org/1999/xlink');
  $writer->writeAttribute('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance');
  $writer->writeAttribute('xmlns:mads', 'http://www.loc.gov/mads/v2');

  return $writer;
}

/**
 * Get a MODS xml writer.
 */
function utailla_get_mods_writer() {
  $writer = new XMLWriter();
  $writer->openMemory();
  $writer->startDocument('1.0', 'utf-8');
  $writer->startElement('mods');
  $writer->writeAttribute('xmlns', 'http://www.loc.gov/mods/v3');
  $writer->writeAttribute('xmlns:xlink', 'http://www.w3.org/1999/xlink');
  $writer->writeAttribute('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance');
  $writer->writeAttribute('xmlns:mods', 'http://www.loc.gov/mods/v3');

  return $writer;
}

/**
 * Sets the user relations on an object based on the originating DB row.
 */
function utailla_set_user_rels(AbstractObject $object, $row) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  // Link to user.
  if ($row['user_id']) {
    $uid = utailla_get_migrated_user_uid($row['user_id']);
    if ($uid) {
      $object->relationships->add(
        ISLANDORA_RELS_EXT_URI,
        UTAILLA_USER_REL,
        utailla_get_uri_from_user($uid)
      );
    }
  }
  // Link to contact user.
  if ($row['contact_id']) {
    $uid = utailla_get_migrated_contact_uid($row['contact_id']);
    if ($uid) {
      $object->relationships->add(
        ISLANDORA_RELS_EXT_URI,
        UTAILLA_CONTACT_REL,
        utailla_get_uri_from_user($uid)
      );
    }
  }
}

/**
 * Concatenate name parts based on a DB row.
 */
function utailla_concat_name_parts($db_row) {
  $name = '';
  $row = (object) $db_row;
  if ($row->first_name && !$row->last_name) {
    $name = $row->first_name;
  }
  elseif ($row->last_name && !$row->first_name) {
    $name = $row->last_name;
  }
  elseif ($row->last_name && $row->first_name) {
    $name = "{$row->first_name} {$row->last_name}";
  }
  return $name;
}

/**
 * Writes common MADS elements based on a db row.
 */
function utailla_write_common_mads($writer, $row) {
  // English description.
  if (trim($row['description_en'])) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'biographical/historical');
    $writer->writeAttribute('lang', 'eng');
    $writer->text($row['description_en']);
    $writer->endElement();
  }
  // Spanish description.
  if (trim($row['description_sp'])) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'biographical/historical');
    $writer->writeAttribute('lang', 'spa');
    $writer->text($row['description_sp']);
    $writer->endElement();
  }
  // Depositor status.
  if ($row['depositor']) {
    $writer->startElement('note');
    $writer->writeAttribute('type', 'depositor status');
    if ($row['depositor'] == 'Y') {
      $writer->text('yes');
    }
    else {
      $writer->text('no');
    }
    $writer->endElement();
  }
}

/**
 * Writes a language MADS element.
 */
function utailla_write_mads_language($writer, $language_id, $type = 'research language') {
  $pid = utailla_migrated_language_pid($language_id);
  if ($pid) {
    $writer->startElement('fieldOfActivity');
    $writer->writeAttribute('type', $type);
    $writer->writeAttribute('authority', 'aillaLanguage');
    $writer->writeAttribute('valueURI', utailla_get_uri_from_pid($pid));
    $writer->endElement();
  }
}

/**
 * Get the array of access justification options.
 */
function utailla_access_justification_options() {
  return array(
    t('Protected population (e.g., children)'),
    t('Required by IRB or other authority'),
    t('Access restricted by speech community'),
    t("Wish of speaker or speaker's family"),
    t('Ceremonial, ritual, or esoteric language'),
    t('Thesis in progress'),
    t('Material under copyright'),
    t('Could not obtain informed consent'),
    t('Other reason: please explain'),
  );
}

/**
 * Get a list of lockable DSIDs.
 */
function utailla_lockable_dastastreams() {
  return array(
    'OBJ',
    'MEDIUM_SIZE',
    'JPG',
    'JP2',
    'OGG',
    'MKV',
    'MP4',
    'PDF',
    'PROXY_MP3',
    'PDFA',
    'FULL_TEXT',
    'PREVIEW',
  );
}

/**
 * Set the restriction on a file object.
 */
function utailla_set_restriction(AbstractObject $object, $restriction_info) {
  module_load_include('inc', 'utailla', 'includes/db');
  $lockable_dsids = utailla_lockable_dastastreams();
  $xacml = new IslandoraXacml($object);
  if ($restriction_info['level'] == 0 && !is_null($restriction_info['level'])) {
    foreach ($lockable_dsids as $dsid) {
      $xacml->datastreamRule->addDsid($dsid);
    }
    $xacml->datastreamRule->addRole('authenticated user');
  }
  else {
    $xacml->datastreamRule->clear();
  }
  $xacml->writeBackToFedora();
  utailla_upsert_restriction($restriction_info + array(
    'pid' => $object->id,
  ));
}

/**
 * Gets any current authorization for the current user and PID.
 */
function utailla_user_media_file_current_authorization($pid) {
  module_load_include('inc', 'utailla', 'includes/db');
  $authorization = utailla_user_media_file_authorization($pid);
  if ($authorization &&
    time() < ($authorization['authorization_time'] + UTAILLA_AUTHORIZATION_DURATION)) {
    return $authorization;
  }
  elseif ($authorization) {
    utailla_remove_user_media_file_authorization($pid);
  }
  return FALSE;
}

/**
 * Updates the XACML policy for contributors as needed.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing within Fedora.
 * @param bool $anonymous
 *   TRUE if anonymous, FALSE otherwise.
 */
function utailla_update_contributor_anonymous_policy(AbstractObject $object, $anonymous) {
  $xacml = new IslandoraXacml($object);
  $updates_needed = FALSE;
  // Fresh XACML go to town on naively adding things.
  if ($anonymous) {
    if (!isset($object['POLICY'])) {
      $xacml->datastreamRule->addDsid('MADS');
      $xacml->datastreamRule->addUser($object->owner);
      $xacml->datastreamRule->addRole('administrator');
      $updates_needed = TRUE;
    }
    else {
      // XACML already exists need to append to potentially.
      if (!in_array('MADS', $xacml->datastreamRule->getDsids())) {
        $xacml->datastreamRule->addDsid('MADS');
        $updates_needed = TRUE;
      }
      if (!in_array($object->owner, $xacml->datastreamRule->getUsers())) {
        $xacml->datastreamRule->addUser($object->owner);
        $updates_needed = TRUE;
      }
      if (!in_array('administrator', $xacml->datastreamRule->getRoles())) {
        $xacml->datastreamRule->addRole('administrator');
        $updates_needed = TRUE;
      }
    }
  }
  else {
    // Need to see if we need to remove things from the XACML datastream rule.
    if (isset($object['POLICY'])) {
      $ds_rule = $xacml->datastreamRule->getRuleArray();
      if (!empty($ds_rule['mimes']) || !empty($ds_rule['dsidregexs']) || !empty($ds_rule['mimeregexs']) || count($ds_rule['dsids']) > 1) {
        drupal_set_message(t('Datastream POLICY settings have diverged from default: Restriction for MADS has not been automatically lifted for @pid.', array('@pid' => $object->id)), 'error');
      }
      elseif (in_array('MADS', $ds_rule['dsids'])) {
        $updates_needed = TRUE;
        $xacml->datastreamRule->removeDsid('MADS');
        $xacml->datastreamRule->removeUser($object->owner);
        // Let's see if the entire POLICY can be nuked.
        if (!$xacml->managementRule->isPopulated() && !$xacml->viewingRule->isPopulated() && !$xacml->datastreamRule->isPopulated()) {
          $object->purgeDatastream('POLICY');
          $updates_needed = FALSE;
        }
      }
    }
  }
  if ($updates_needed) {
    $xacml->writeBackToFedora();
  }
}

/**
 * Check the mods to see if an object is archived.
 *
 * @param AbstractObject $object
 *   The fedora object to check for archival status.
 *
 * @return bool
 *   TRUE if archival status is A, FALSE otherwise.
 */
function utailla_is_object_archived(AbstractObject $object) {
  if (isset($object['MODS'])) {
    $mods = new SimpleXMLElement($object['MODS']->content);
    $mods->registerXPathNamespace('mods', 'http://www.loc.gov/mods/v3');
    // Archival Status.
    $archival_status = $mods->xpath('/mods:mods/mods:note[@type="archival status"]');
    if (count($archival_status) > 0) {
      $status = (string) reset($archival_status);
      if (!empty($status) && $status == "A") {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 * Gets any namepart data from contributor object MADS datastream.
 */
function utailla_get_contributor_nameparts_from_mads($mads_ds) {
  $results = array(
    "given_name" => "",
    "family_name" => "",
  );
  // Load contributor MADS Datastream.
  $dom = new DOMDocument();
  $dom->loadXML($mads_ds);
  $xpath = new DOMXPath($dom);
  $xpath->registerNamespace('mads', 'http://www.loc.gov/mads/v2');
  // XXX: This assumes that the contributor objects will only ever have one MADS
  // record for name, note[@type="anonymous"], and mads:note[@type="nickname"].
  $results['given_name'] = $xpath->evaluate('string(/mads:mads/mads:authority/mads:name[@type="personal"]/mads:namePart[@type="given"]/text())');
  $results['family_name'] = $xpath->evaluate('string(/mads:mads/mads:authority/mads:name[@type="personal"]/mads:namePart[@type="family"]/text())');
  // Anonymous record(s) use nickname if avialable otherwise defaults to
  // 'Anonymous' and saves the value into the namepart[@type="family"].
  $anonymous = $xpath->evaluate('/mads:mads/mads:note[@type="anonymous"]/text() = "Y"');
  if ($anonymous) {
    $results['given_name'] = "";
    $nickname = $xpath->evaluate('string(/mads:mads/mads:note[@type="nickname"]/text())');
    if (!empty($nickname)) {
      $results['family_name'] = $nickname;
    }
    else {
      $results['family_name'] = 'Anonymous';
    }
  }
  return $results;
}

/**
 * Helper to get a new object with a bound PID.
 *
 * @return IslandoraNewFedoraObject
 *   An uningested object with a concrete PID.
 */
function utailla_get_new_object($namespace = 'ailla') {
  $tuque = islandora_get_tuque_connection();
  $pid = $tuque->repository->getNextIdentifier($namespace);
  return $tuque->repository->constructObject($pid);
}

/**
 * Add a restriction entry to the object if one is not present.
 *
 * @param AbstractObject $object
 *   The object to which to add a restriction. If it has not yet been allocated
 *   a PID, then we skip it and do nothing. PIDs might be allocated by being
 *   either constructed similar to utailla_get_new_object() or by having already
 *   been ingested into Fedora.
 */
function utailla_add_restriction_if_missing(AbstractObject $object) {
  module_load_include('inc', 'utailla', 'includes/db');
  module_load_include('inc', 'utailla', 'includes/access_levels.form');
  module_load_include('inc', 'islandora', 'includes/utilities');

  if (!islandora_is_valid_pid($object->id)) {
    // Object does not yet have a real PID allocated; abort.
    return;
  }

  // See if this object already has a restriction and if it does ignore it.
  $restriction = FALSE;
  // Contributors don't need to have access level restrictions applied to them.
  if (!in_array(UTAILLA_CONTRIBUTOR_CMODEL, $object->models) && $object->id != UTAILLA_CONTRIBUTOR_COLLECTION && !utailla_get_restriction($object->id)) {
    // See if the collection the media file is a part of has a restriction set.
    // XXX: Is technically possible for a media file to not be part of a
    // resource.
    $ignore_models = array_merge(utailla_collection_like_models(), array('fedora-system:ContentModel-3.0'));
    if (!array_intersect($ignore_models, $object->models)) {
      $parent_rels = $object->relationships->get(FEDORA_RELS_EXT_URI, 'isMemberOfCollection');
      $parent_rels = reset($parent_rels);
      $parent_pid = $parent_rels['object']['value'];
      $parent_object = islandora_object_load($parent_pid);
      // See if this a collection or if we need to go one more level.
      if (in_array('islandora:collectionCModel', $object->models)) {
        $restriction = utailla_get_collection_restriction($parent_object->id);
      }
      else {
        $grandparent_rels = $parent_object->relationships->get(FEDORA_RELS_EXT_URI, 'isMemberOfCollection');
        $grandparent_rels = reset($grandparent_rels);
        $grandparent_pid = $grandparent_rels['object']['value'];
        $restriction = utailla_get_collection_restriction($grandparent_pid);
      }
      if ($restriction) {
        utailla_set_restriction($object, array(
          'level' => UTAILLA_RESTRICTION_LEVEL_1,
          'hint' => $restriction['hint'],
          'password' => $restriction['password'],
        ));
      }
    }
    if (!$restriction) {
      utailla_set_restriction($object, array('level' => UTAILLA_RESTRICTION_LEVEL_1));
    }
  }
}

/**
 * Queries RI for all language object PIDs.
 *
 * @return array
 *   RI query results.
 */
function utailla_get_language_pids() {
  $islandora_tuque = islandora_get_tuque_connection();
  $parent_query = <<<EOQ
SELECT ?pid ?title FROM <#ri>
WHERE {
?pid <fedora-model:hasModel> <info:fedora/ailla:languageCModel> ;
     <fedora-model:label> ?title
}
EOQ;
  // XXX: If we ever index the AILLA datastreams this should move to Solr
  // for this entire autocomplete to cut down on Fedora hits.
  return $islandora_tuque->repository->ri->sparqlQuery($parent_query, 'unlimited');
}
