<?php
/**
 * @file
 * General autocomplete functionality for utailla.
 */

/**
 * Autocomplete that searches by title within a specific model.
 *
 * @param string $string
 *   A search string.
 * @param string $model
 *   A string representing the Fedora content model.
 *
 * @return string
 *   The rendered JSON results.
 */
function utailla_title_by_model_autocomplete($string, $model) {
  $matches = array();
  $islandora_tuque = islandora_get_tuque_connection();
  $parent_query = <<<EOQ
SELECT ?pid ?title FROM <#ri>
WHERE {
?pid <fedora-model:hasModel> <info:fedora/$model> ;
     <fedora-model:label> ?title
}
EOQ;
  // XXX: If we ever index the AILLA datastreams this should move to Solr
  // for this entire autocomplete to cut down on Fedora hits.
  $results = $islandora_tuque->repository->ri->sparqlQuery($parent_query, 'unlimited');

  $string_filter = function($r) use($string) {
    return strpos(drupal_strtoupper($r['title']['value']), drupal_strtoupper($string)) !== FALSE;
  };
  $language_filter = function($r) {
    // Need to see if these are part of the aillaLangFam as per UTAILLA-44.
    $object = islandora_object_load($r['pid']['value']);
    if (isset($object['LANGUAGE'])) {
      $dom = new DOMDocument();
      $dom->loadXML($object['LANGUAGE']->content);
      $dom_xpath = new DOMXPath($dom);
      return $dom_xpath->evaluate('/aillaLanguage/aillaLangFam/text() = "Y"');
    }
    return FALSE;
  };
  $results = array_filter($results, $string_filter);
  if ($model == 'ailla:languageCModel') {
    $results = array_filter($results, $language_filter);
  }
  foreach ($results as $result) {
    $matches[$result['title']['value']] = $result['title']['value'] . ' (' . $result['pid']['value'] . ')';
  }
  drupal_json_output($matches);
}

/**
 * Autocomplete that searches for Contributor objects.
 *
 * @param string $string
 *   A search string.
 *
 * @return string
 *   The rendered JSON results.
 */
function utailla_associate_contributor_autocomplete($string) {
  $qp = new IslandoraSolrQueryProcessor();
  $qp->solrLimit = 0;
  $contributor_search_label_field = variable_get('utailla_contributor_search_field', 'fgs_label_t');
  $display_label_field = variable_get('islandora_solr_object_label_field', 'fgs_label_s');
  $model_field = variable_get('islandora_solr_content_model_field', 'RELS_EXT_hasModel_uri_ms');
  $qp->buildQuery(format_string("($model_field:\"info:fedora/@contributor_cmodel\" AND $contributor_search_label_field:*$string*) OR ($model_field:\"info:fedora/@organization_cmodel\" AND $contributor_search_label_field:*$string*)", array(
    '@contributor_cmodel' => UTAILLA_CONTRIBUTOR_CMODEL,
    '@organization_cmodel' => UTAILLA_ORG_CMODEL,
  )));
  $qp->executeQuery();
  $matches = array();
  if ($qp->islandoraSolrResult['response']['numFound'] > 0) {
    foreach ($qp->islandoraSolrResult['response']['objects'] as $object) {
      $matches["{$object['solr_doc'][$display_label_field]} ({$object['PID']})"] = "{$object['solr_doc'][$display_label_field]} ({$object['PID']})";
    }
  }
  drupal_json_output($matches);
}
