<?php
/**
 * @file
 * Holds functions related to the updating of metadata in a Drush batch.
 */


/**
 * Constructs a batch to update all the metadatas.
 */
function utailla_update_metadata_batch() {
  return array(
    'operations' => array(
      array(
        'utailla_update_general_metadata_batch_operation',
        array('islandora:collectionCModel', 'MODS'),
      ),
      array(
        'utailla_update_general_metadata_batch_operation',
        array(UTAILLA_CONTRIBUTOR_CMODEL, 'MADS'),
      ),
      array(
        'utailla_update_general_metadata_batch_operation',
        array(UTAILLA_RESOURCE_CMODEL, 'MODS'),
      ),
      array(
        'utailla_update_general_metadata_batch_operation',
        array(UTAILLA_ORG_CMODEL, 'MADS'),
      ),
      array(
        'utailla_update_general_metadata_batch_operation',
        array(UTAILLA_LANGUAGE_CMODEL, 'LANGUAGE'),
      ),
      array(
        'utailla_update_general_metadata_batch_operation',
        array('media', 'MODS'),
      ),
    ),
    'title' => t('Updating metadata for AILLA...'),
    'init_message' => t('Preparing to update metadata.'),
    'progress_message' => t('Time elapsed: @elapsed <br/>Estimated time remaining @estimate.'),
    'error_message' => t('An error has occurred.'),
    'file' => drupal_get_path('module', 'utailla') . '/includes/update.batch.inc',
  );
}

/**
 * General metadata batch operation.
 *
 * @param string $cmodel
 *   A string representing a content model within Fedora.
 * @param string $dsid
 *   A string representing a datastream ID of the metadata.
 * @param array $context
 *   An array containing the context of the current batch.
 */
function utailla_update_general_metadata_batch_operation($cmodel, $dsid, &$context) {
  // Create a mapping of callback operations to be called on content models.
  $mapping = array(
    'islandora:collectionCModel' => 'utailla_update_collection_object',
    UTAILLA_CONTRIBUTOR_CMODEL => 'utailla_update_contributor_object',
    UTAILLA_RESOURCE_CMODEL => 'utailla_update_resource_object',
    UTAILLA_ORG_CMODEL => 'utailla_update_organization_object',
    UTAILLA_LANGUAGE_CMODEL => 'utailla_update_language_object',
    'media' => 'utailla_update_media_object',
  );

  $sandbox = &$context['sandbox'];

  drupal_static_reset('islandora_get_tuque_connection');
  $tuque = islandora_get_tuque_connection();

  $query = utailla_helper_set_query($cmodel, $dsid);
  if (!isset($sandbox['total'])) {
    $sandbox['progress'] = 0;
    $sandbox['total'] = $tuque->repository->ri->countQuery(format_string($query, array(
      '!filter' => '',
    )), 'sparql');
    if ($sandbox['total'] == 0) {
      // Nothing to process.
      $context['finished'] = 1;
      $context['message'] = t('Nothing to fix.');
      return;
    }
    $sandbox['result_stash'] = array();
  }
  if (empty($sandbox['result_stash'])) {
    $limited_query = format_string($query, array(
        '!filter' => (isset($sandbox['last_date']) ?
            "FILTER(?created > \"{$sandbox['last_date']}\"^^<xsd:dateTime>)" :
            ''),
      )) . <<<EOQ
LIMIT 1000
EOQ;
    drush_log(dt('Querying for 1000 objects.'));
    $sandbox['result_stash'] = $tuque->repository->ri->sparqlQuery($limited_query);
    if (empty($sandbox['result_stash'])) {
      // Ran out of items early?
      $context['finished'] = 1;
      $context['message'] = t('Updated @count PID(s) and ran out of items early... Somebody manually updated a document?', array(
        '@count' => $sandbox['total'],
      ));
      return;
    }
  }
  $result = array_shift($sandbox['result_stash']);
  $updating_pid = $result['object']['value'];
  $sandbox['last_date'] = $result['created']['value'];
  $object_to_update = islandora_object_load($updating_pid);
  call_user_func($mapping[$cmodel], $object_to_update);
  $sandbox['progress'] = min($sandbox['total'], $sandbox['progress'] + 1);
  $context['finished'] = $sandbox['progress'] / $sandbox['total'];
  $context['message'] = t('Updated @progress of @total.', array(
    '@progress' => $sandbox['progress'],
    '@total' => $sandbox['total'],
  ));
}

/**
 * Helper to keep it DRY to create the base query.
 *
 * @param string $cmodel
 *   String of the content model to construct the query for.
 * @param string $dsid
 *   String of the metadata datastream ID being updated.
 *
 * @return string
 *   The formatted query.
 */
function utailla_helper_set_query($cmodel, $dsid) {
  if ($cmodel == 'media') {
    $query = <<<EOQ
    SELECT ?object ?created
FROM <#ri>
WHERE {
  ?object <fedora-model:hasModel> ?model ;
          <fedora-model:createdDate> ?created ;
          <fedora-view:disseminates> ?ds .
  ?ds <fedora-view:disseminationType> <info:fedora/*/!dsid> .
  FILTER(sameTerm(?model, <info:fedora/islandora:sp-audioCModel>) || sameTerm(?model, <info:fedora/islandora:sp_pdf>) || sameTerm(?model, <info:fedora/islandora:sp_document>) || sameTerm(?model, <info:fedora/islandora:sp_basic_image>) || sameTerm(?model, <info:fedora/islandora:sp_large_image_cmodel>) || sameTerm(?model, <info:fedora/islandora:binaryObjectCModel>) || sameTerm(?model, <info:fedora/islandora:videoCModel>))
}
ORDER BY ?created

EOQ;
  }
  else {
    $query = <<<EOQ
SELECT ?object ?created
FROM <#ri>
WHERE {
  ?object <fedora-model:hasModel> <info:fedora/!cmodel> ;
          <fedora-model:createdDate> ?created ;
          <fedora-view:disseminates> ?ds .
  ?ds <fedora-view:disseminationType> <info:fedora/*/!dsid> .
  !filter
}
ORDER BY ?created

EOQ;
  }
  return format_string($query, array(
    '!cmodel' => $cmodel,
    '!dsid' => $dsid,
  ));
}

/**
 * Update a collection object.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing a collection object.
 */
function utailla_update_collection_object(AbstractObject $object) {
  $dom = utailla_get_metadata_dom($object['MODS']->content);
  $xpath = new DOMXPath($dom);
  $xpath->registerNamespace('mods', 'http://www.loc.gov/mods/v3');

  // Remove the namePart elements.
  $name_changed = utailla_remove_namepart_from_mods($xpath);
  // Add a mods:identifier element.
  $identifier_changed = utailla_update_mods_identifier($dom, $xpath, $object->id);

  if ($name_changed || $identifier_changed) {
    $object['MODS']->content = $dom->saveXML();
    drush_log(dt('Updated the MODS for the collection object @pid.', array('@pid' => $object->id)));
  }
  else {
    drush_log(dt('No updates to be done for @pid.', array('@pid' => $object->id)));
  }
}

/**
 * Update a contributor object.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing a collection object.
 */
function utailla_update_contributor_object(AbstractObject $object) {
  $dom = utailla_get_metadata_dom($object['MADS']->content);
  $xpath = new DOMXPath($dom);
  $xpath->registerNamespace('mads', 'http://www.loc.gov/mads/v2');

  $nick_xpath = $xpath->query('/mads:mads/mads:note[@type="nickname"]');
  $nick_updated = FALSE;
  if ($nick_xpath->length > 0) {
    $nick = $nick_xpath->item(0);
    $nick->setAttribute('type', 'pseudonym');
    $nick_updated = TRUE;
  }
  $depositor_updated = utailla_update_mads_depositor($dom, $xpath);
  $identifier_updated = utailla_update_mads_identifier($dom, $xpath, $object->id);

  // Organizations need to live underneath a separate affiliation.
  $affiliations = $xpath->query('/mads:mads/mads:affiliation');
  $affiliations_updated = FALSE;
  foreach ($affiliations as $affiliation) {
    $to_remove = $xpath->query('mads:organization[position() > 1]', $affiliation);
    foreach ($to_remove as $element) {
      // Remove and re-add.
      $affiliation->removeChild($element);
      $new_affiliation = $dom->createElement('affiliation');
      $new_affiliation->appendChild($element);
      $dom->documentElement->appendChild($new_affiliation);
      $affiliations_updated = TRUE;
    }
  }

  if ($nick_updated || $depositor_updated || $identifier_updated || $affiliations_updated) {
    $object['MADS']->content = $dom->saveXML();
    drush_log(dt('Updated the MADS for the contributor object @pid.', array('@pid' => $object->id)));
  }
  else {
    drush_log(dt('No updates to be done for @pid.', array('@pid' => $object->id)));
  }
}

/**
 * Update a resource object.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing a collection object.
 */
function utailla_update_resource_object(AbstractObject $object) {
  $dom = utailla_get_metadata_dom($object['MODS']->content);
  $xpath = new DOMXPath($dom);
  $xpath->registerNamespace('mods', 'http://www.loc.gov/mods/v3');

  $name_changed = utailla_remove_namepart_from_mods($xpath);
  $identifier_changed = utailla_update_mods_identifier($dom, $xpath, $object->id);

  $lang_changed = FALSE;
  $lang_attribute = $xpath->query('/mods:mods/mods:titleInfo[@displayLabel = "Title (Indigenous language)" and @type = "alternative" and not(@lang = "?")]');
  if ($lang_attribute->length > 0) {
    $node = $lang_attribute->item(0);
    $node->setAttribute('lang', '?');
    $lang_changed = TRUE;
  }
  if ($name_changed || $identifier_changed || $lang_changed) {
    $object['MODS']->content = $dom->saveXML();
    drush_log(dt('Updated the MODS for the resource object @pid.', array('@pid' => $object->id)));
  }
  else {
    drush_log(dt('No updates to be done for @pid.', array('@pid' => $object->id)));
  }
}

/**
 * Update a organization object.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing a collection object.
 */
function utailla_update_organization_object(AbstractObject $object) {
  $dom = utailla_get_metadata_dom($object['MADS']->content);
  $xpath = new DOMXPath($dom);
  $xpath->registerNamespace('mads', 'http://www.loc.gov/mads/v2');
  $depositor_updated = utailla_update_mads_depositor($dom, $xpath);
  $mads_updated = utailla_update_mads_identifier($dom, $xpath, $object->id);
  if ($depositor_updated || $mads_updated) {
    $object['MADS']->content = $dom->saveXML();
    drush_log(dt('Updated the MADS for the organization object @pid.', array('@pid' => $object->id)));
  }
  else {
    drush_log(dt('No updates to be done for @pid.', array('@pid' => $object->id)));
  }
}

/**
 * Update a language object.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing a collection object.
 */
function utailla_update_language_object(AbstractObject $object) {
  $dom = utailla_get_metadata_dom($object['LANGUAGE']->content);
  $xpath = new DOMXPath($dom);

  // AILLA wants to rename two of their elements.
  $to_rename = array(
    'aillaMacroFam' => '/aillaLanguage/aillaLangFam[text() = "Y" or text() = "N"]',
    'aillaLangFam' => '/aillaLanguage/aillaParentLang',
  );
  $elements_updated = FALSE;
  foreach ($to_rename as $element_name => $lang_xpath) {
    $result = $xpath->query($lang_xpath);
    if ($result->length > 0) {
      // Remove the old element and make a new one.
      $old_element = $result->item(0);
      $element_value = $old_element->nodeValue;
      $old_element->parentNode->removeChild($old_element);
      $new_element = $dom->createElement($element_name);
      $new_element->nodeValue = $element_value;
      $dom->documentElement->appendChild($new_element);
      $elements_updated = TRUE;
    }
  }
  $identifier_updated = FALSE;
  $identifier = $xpath->query('/aillaLanguage/recordInfo/recordIdentifier');
  if ($identifier->length == 0) {
    $record_info_element = $dom->createElement('recordInfo');
    $record_identifier_element = $dom->createElement('recordIdentifier');
    $record_identifier_element->nodeValue = $object->id;
    $record_info_element->appendChild($record_identifier_element);
    $dom->documentElement->appendChild($record_info_element);
    $identifier_updated = TRUE;
  }
  if ($elements_updated || $identifier_updated) {
    $object['LANGUAGE']->content = $dom->saveXML();
    drush_log(dt('Updated the LANGUAGE for the language object @pid.', array('@pid' => $object->id)));
  }
  else {
    drush_log(dt('No updates to be done for @pid.', array('@pid' => $object->id)));
  }
}

/**
 * Update a media object.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing a collection object.
 */
function utailla_update_media_object(AbstractObject $object) {
  $dom = utailla_get_metadata_dom($object['MODS']->content);
  $xpath = new DOMXPath($dom);
  $xpath->registerNamespace('mods', 'http://www.loc.gov/mods/v3');

  // Add a mods:identifier element.
  $dom_changed = utailla_update_mods_identifier($dom, $xpath, $object->id);
  if ($dom_changed) {
    $object['MODS']->content = $dom->saveXML();
    drush_log(dt('Updated the MODS for the media object @pid.', array('@pid' => $object->id)));
  }
  else {
    drush_log(dt('No updates to be done for @pid.', array('@pid' => $object->id)));
  }
}

/**
 * Helper which instansiates a DOMDocument.
 *
 * @param string $xml
 *   The XML to be loaded.
 *
 * @return DOMDocument
 *   A DOMDocument to be used with the XML loaded.
 */
function utailla_get_metadata_dom($xml) {
  $dom = new DOMDocument();
  $dom->preserveWhiteSpace = FALSE;
  $dom->formatOutput = TRUE;
  $dom->loadXML($xml);
  return $dom;
}

/**
 * Helper which removes the mods:namePart elements from a DOMDocument.
 *
 * @param DOMXPath $xpath
 *   A DOMXPath to be queried.
 *
 * @return bool
 *   Whether namePart element was removed or not.
 */
function utailla_remove_namepart_from_mods($xpath) {
  $name_changed = FALSE;
  $name_part = $xpath->query('/mods:mods/mods:name/mods:namePart');
  foreach ($name_part as $part) {
    $part->parentNode->removeChild($part);
    $name_changed = TRUE;
  }
  return $name_changed;
}

/**
 * Helper which adds a mods:identifier element to a DOMDocument.
 *
 * @param DOMDocument $dom
 *   A DOMDocument to be modified.
 * @param DOMXPath $xpath
 *   A DOMXPath to be queried.
 * @param string $pid
 *   The PID of the Fedora object to be added.
 *
 * @return bool
 *   TRUE if the DOM is to be updated, FALSE otherwise.
 */
function utailla_update_mods_identifier(&$dom, $xpath, $pid) {
  $identifier = $xpath->query('/mods:mods/mods:identifier[@type="URI"]');
  if ($identifier->length == 0) {
    module_load_include('inc', 'utailla', 'includes/utilities');
    $element = $dom->createElement('identifier');
    $element->setAttribute('type', 'URI');
    $element->nodeValue = utailla_get_uri_from_pid($pid);
    $dom->documentElement->appendChild($element);
    return TRUE;
  }
  return FALSE;
}

/**
 * Helper which adds a mads:recordIdentifier to a DOMDocument.
 *
 * @param DOMDocument $dom
 *   A DOMDocument to be modified.
 * @param DOMXPath $xpath
 *   A DOMXPath to be queried.
 * @param string $pid
 *   The PID of the Fedora object to be added.
 *
 * @return bool
 *   TRUE if the DOM is to be updated, FALSE otherwise.
 */
function utailla_update_mads_identifier(&$dom, $xpath, $pid) {
  $identifier = $xpath->query('/mads:mads/mads:recordInfo/mads:recordIdentifier');
  if ($identifier->length == 0) {
    module_load_include('inc', 'utailla', 'includes/utilities');
    $record_info_element = $dom->createElement('recordInfo');
    $record_identifier_element = $dom->createElement('recordIdentifier');
    $record_identifier_element->nodeValue = $pid;
    $record_info_element->appendChild($record_identifier_element);
    $dom->documentElement->appendChild($record_info_element);
    return TRUE;
  }
  return FALSE;
}

/**
 * Helper which updates the depositor element from yes/no to y/n.
 *
 * @param DOMDocument $dom
 *   A DOMDocument to be modified.
 * @param DOMXPath $xpath
 *   A DOMXPath to be queried.
 *
 * @return bool
 *   TRUE if the DOM is to be updated, FALSE otherwise.
 */
function utailla_update_mads_depositor(&$dom, $xpath) {
  $depositor = $xpath->query('/mads:mads/mads:note[@type="depositor status"][text() = "yes" or text() = "no"]');
  if ($depositor->length > 0) {
    $mapping = array(
      'yes' => 'Y',
      'no' => 'N',
    );
    $node = $depositor->item(0);
    $node->nodeValue = $mapping[$node->nodeValue];
    return TRUE;
  }
  return FALSE;
}
