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
  $results = $islandora_tuque->repository->ri->sparqlQuery($parent_query, 'unlimited');

  foreach ($results as $result) {
    if (strpos(drupal_strtoupper($result['title']['value']), drupal_strtoupper($string)) !== FALSE) {
      // Need to see if these are part of the aillaLangFam as per UTAILLA-44.
      // XXX: If we ever index the language datastream this should move to Solr
      // for this entire autocomplete to cut down on Fedora hits.
      $object = islandora_object_load($result['pid']['value']);
      if (isset($object['LANGUAGE'])) {
        $dom = new DOMDocument();
        $dom->loadXML($object['LANGUAGE']->content);
        $dom_xpath = new DOMXPath($dom);
        if ($dom_xpath->evaluate('/aillaLanguage/aillaLangFam/text() = "Y"')) {
          $matches[$result['title']['value']] = $result['title']['value'] . ' (' . $result['pid']['value'] . ')';
        }
      }
    }
  }
  drupal_json_output($matches);
}
