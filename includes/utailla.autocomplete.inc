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
 * @param bool $filter_by_parent
 *   TRUE if you want to filter by parent collection.
 *
 * @return string
 *   The rendered JSON results.
 */
function utailla_title_by_model_autocomplete($string, $model, $filter_by_parent = TRUE) {
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
  if ($model == 'ailla:languageCModel' && $filter_by_parent) {
    $results = array_filter($results, $language_filter);
  }
  foreach ($results as $result) {
    $matches[$result['title']['value']] = $result['title']['value'] . ' (' . $result['pid']['value'] . ')';
  }
  drupal_json_output($matches);
}

/**
 * Autocomplete that searches for language codes.
 *
 * @param string $string
 *   A search string.
 *
 * @return string
 *   The rendered JSON results.
 */
function utailla_language_code_autocomplete($string) {
  module_load_include('inc', 'utailla', 'includes/utilities');
  $results = utailla_get_language_pids();

  $language_code_map = function($r) {
    // Need to see if these are part of the aillaLangFam as per UTAILLA-44.
    $object = islandora_object_load($r['pid']['value']);
    if (isset($object['LANGUAGE'])) {
      $dom = new DOMDocument();
      $dom->loadXML($object['LANGUAGE']->content);
      $dom_xpath = new DOMXPath($dom);
      $lang_codes = $dom_xpath->query('/aillaLanguage/aillaLangCode/text()');
      foreach ($lang_codes as $lang_code) {
        $mapped = array();
        $mapped[$lang_code->wholeText] = $lang_code->wholeText . ' (' . $r['pid']['value'] . ')';
        return $mapped;
      }
    }
    return NULL;
  };
  $null_filter = function($r) {
    return $r !== NULL;
  };
  $codes = array_map($language_code_map, $results);
  $codes = array_filter($codes, $null_filter);

  $matches = array();
  foreach ($codes as $code) {
    foreach ($code as $key => $value) {
      $matches[$key] = $value;
    }
  }
  drupal_json_output($matches);
}

/**
 * Autocomplete that searches for Contributor/Organization objects.
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
