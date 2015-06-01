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
 */
function utailla_get_decoded_row_recursive($row_object) {
  $row_array = (array) $row_object;
  foreach ($row_array as $index => $column) {
    if (is_array($column)) {
      foreach ($column as $key => $value) {
        $row_array[$index][$key] = utailla_get_decoded_row_recursive($value);
      }
    }
    else {
      $row_array[$index] = preg_replace(
        '/<br\s*\/?>/i',
        "\n",
        html_entity_decode($column, ENT_QUOTES, 'UTF-8')
      );
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
 * Get a PID from a site specific URI.
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
