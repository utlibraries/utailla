<?php
/**
 * @file
 * Handles the ingestion of child objects to resources.
 */

/**
 * Configuration for ingesting a resource using ingest steps.
 *
 * @param AbstractObject $resource
 *   An AbstractObject representing a resource object within Fedora.
 *
 * @return array
 *   An array containing the ingest configuration, an empty array if none
 *   applicable.
 *
 * @see islandora_basic_collection_get_ingest_configuration()
 */
function utailla_resource_get_ingest_configuration(AbstractObject $resource) {
  $configuration = array();
  if (isset($resource['COLLECTION_POLICY'])) {
    module_load_include('inc', 'islandora_basic_collection', 'includes/ingest.form');
    // The ISLANDORA_BASIC_COLLECTION_FAKE_PID is needed such that
    // constructObject() won't call getNextIdentifier(), needlessly.
    $object = $resource->repository->constructObject(ISLANDORA_BASIC_COLLECTION_FAKE_PID);
    $object->label = 'New Object';

    module_load_include('inc', 'islandora_basic_collection', 'includes/utilities');
    islandora_basic_collection_add_to_collection($object, $resource);
    $policy = new CollectionPolicy($resource['COLLECTION_POLICY']->content);
    $configuration = array(
      'collection' => $resource,
      'models' => array_keys($policy->getContentModels()),
      'objects' => array($object),
      'parent' => $resource->id,
    );
  }
  return $configuration;
}

/**
 * Manage action that for ingestion of an object into the given collection.
 *
 * @param AbstractObject $resource
 *   The resource to ingest into.
 *
 * @return array
 *   The ingest form.
 *
 * @see islandora_basic_collection_ingest_action()
 */
function utailla_resource_ingest_action(AbstractObject $resource) {
  $configuration = utailla_resource_get_ingest_configuration($resource);
  module_load_include('inc', 'islandora', 'includes/ingest.form');
  return drupal_get_form('islandora_ingest_form', $configuration);
}
