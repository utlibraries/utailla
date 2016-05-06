<?php
/**
 * @file
 * Derivative callbacks.
 */

/**
 * Ensure a restriction entry is present for the given object.
 */
function utailla_derivative_set_restriction(AbstractObject $object, $force, $hook) {
  // A bit of static caching, to avoid repetitive DB hits.
  $set =& drupal_static(__FUNCTION__, array());

  if (!isset($set[$object->id])) {
    module_load_include('inc', 'utailla', 'includes/utilities');
    utailla_add_restriction_if_missing($object);
    $set[$object->id] = TRUE;
    return array(
      'success' => TRUE,
      'messages' => array(),
    );
  }
}

/**
 * Set an object's label, from the label of its OBJ datastream.
 */
function utailla_derivative_set_label_from_obj(AbstractObject $object, $force, $hook) {
  if ($force || $object->label == 'New Object') {
    $object->label = $object['OBJ']->label;
    return array(
      'success' => TRUE,
      'messages' => array(),
    );
  }
}
