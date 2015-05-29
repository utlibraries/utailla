<?php
/**
 * @file
 * ailla_platform_feature.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function ailla_platform_feature_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_islandora_solr_fields_features_default_fields().
 */
function ailla_platform_feature_islandora_solr_fields_features_default_fields() {
  return array();
}
