<?php
/**
 * @file
 * Access level form and processing.
 */

/**
 * Naively obtain the first "file".
 *
 * @param AbstractObject $object
 *   The object from which to start querying.
 * @param int $slice_size
 *   The number of objects for which to query at a time.
 *
 * @return AbstractObject|NULL
 *   An AbstractObject representing the first file found; otherwise, NULL if we
 *   could not find one.
 */
function utailla_get_first_file(AbstractObject $object, $slice_size = 10) {
  if (array_intersect($object->models, utailla_collection_like_models())) {
    $page = 0;
    list($count, $members) = islandora_basic_collection_get_member_objects($object, $page, $slice_size);
    while (count($members) > 0) {
      foreach ($members as $member) {
        $child = islandora_object_load($member['object']['value']);
        if (!$child) {
          continue;
        }

        return utailla_get_first_file($child, $slice_size);
      }

      $page++;
      list($count, $members) = islandora_basic_collection_get_member_objects($object, $page, $slice_size);
    }
  }
  else {
    return $object;
  }
}

/**
 * Form building function; build out the access level form.
 */
function utailla_access_level_form($form, &$form_state, AbstractObject $object_in) {
  form_load_include($form_state, 'inc', 'utailla', 'includes/access_levels.form');
  module_load_include('inc', 'utailla', 'includes/db');
  module_load_include('inc', 'utailla', 'includes/utilities');

  // XXX: Reload the object to attempt to alleviate any form/caching issues.
  $object = islandora_object_load($object_in->id);

  $first_file = utailla_get_first_file($object);
  $info = (array) utailla_get_restriction($first_file->id) + array(
    'hint' => '',
    'level' => UTAILLA_RESTRICTION_LEVEL_1,
    'reason_option' => 0,
    'custom_reason' => '',
    'end_time' => NULL,
  );
  $form_state['utailla_first_file_info'] = $info;
  $justifications = utailla_access_justification_options();

  if (in_array(UTAILLA_RESOURCE_CMODEL, $object->models)) {
    $files = array();
    list($count, $members) = islandora_basic_collection_get_member_objects($object);
    foreach ($members as $member) {
      $member_info = utailla_get_restriction($member['object']['value']);
      $files[$member['object']['value']] = array(
        'title' => l($member['title']['value'], "islandora/object/{$member['object']['value']}/manage/utailla_access_levels"),
        'modified' => $member['date_modified']['value'],
        'level' => isset($member_info['level']) ? $member_info['level'] : t('Not set'),
        'hint' => isset($member_info['hint']) ? $member_info['hint'] : t('N/A'),
      );
    }
    $form['files'] = array(
      '#type' => 'tableselect',
      '#title' => t('Files'),
      '#header' => array(
        'title' => t('Title'),
        'modified' => t('Date Modified'),
        'level' => t('Current Access Level'),
        'hint' => t('Current Hint'),
      ),
      '#options' => $files,
      '#description' => t('Selecting no files will apply the level to ALL files under this resource. Showing @count of @total.', array(
        '@count' => count($members),
        '@total' => $count,
      )),
      '#theme_wrappers' => array('form_element'),
    );
  }

  $form['level'] = array(
    '#type' => 'radios',
    '#title' => t('Access Level'),
    '#options' => array(
      UTAILLA_RESTRICTION_LEVEL_1 => t('Level 1: Authenticated Access'),
      UTAILLA_RESTRICTION_LEVEL_2 => t('Level 2: Password'),
      UTAILLA_RESTRICTION_LEVEL_3 => t('Level 3: Time limit'),
      UTAILLA_RESTRICTION_LEVEL_4 => t('Level 4: Contact depositor'),
    ),
    '#default_value' => $info['level'],
  );
  $form['password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          '!value' => UTAILLA_RESTRICTION_LEVEL_1,
        ),
      ),
    ),
    '#description' => t('If none is entered and one exists on the first media file object encountered, this discovered password will be applied.'),
  );
  $form['hint'] = array(
    '#type' => 'textfield',
    '#title' => t('Password Hint'),
    '#default_value' => $info['hint'],
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          '!value' => UTAILLA_RESTRICTION_LEVEL_1,
        ),
      ),
    ),
  );
  $form['reason_option'] = array(
    '#type' => 'select',
    '#title' => t('Justification for restricting access'),
    '#options' => $justifications,
    '#default_value' => $info['reason_option'],
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          '!value' => UTAILLA_RESTRICTION_LEVEL_1,
        ),
      ),
    ),
  );
  $form['custom_reason'] = array(
    '#type' => 'textfield',
    '#maxlength' => 256,
    '#title' => t('Explanation for Other justification'),
    '#default_value' => $info['custom_reason'],
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          '!value' => UTAILLA_RESTRICTION_LEVEL_1,
        ),
        ':input[name=reason_option]' => array(
          // XXX: May have to make lookup of "other" option smarter.
          'value' => count($justifications) - 1,
        ),
      ),
    ),
  );
  $form['end_time'] = array(
    '#type' => 'date_popup',
    '#timepicker' => FALSE,
    '#date_format' => 'Y-m-d',
    '#title' => t('Time Limit'),
    '#default_value' => $info['end_time'] !== NULL ? gmdate('Y-m-d', $info['end_time']) : NULL,
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          'value' => UTAILLA_RESTRICTION_LEVEL_3,
        ),
      ),
    ),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Commit changes'),
    ),
  );

  return $form;
}

/**
 * Form validation handler; validate values submitted to the access level form.
 */
function utailla_access_level_form_validate(&$form, &$form_state) {
  module_load_include('inc', 'utailla', 'includes/utilities');
  $justifications = utailla_access_justification_options();
  $values = $form_state['values'];
  $first_restriction = $form_state['utailla_first_file_info'];
  switch ($values['level']) {
    case UTAILLA_RESTRICTION_LEVEL_3:
      if (empty($values['end_time'])) {
        form_error($form['end_time'], t('An ending time must be selected.'));
      }
      elseif (strtotime($values['end_time']) < REQUEST_TIME) {
        form_error($form['end_time'], t('The ending time must be in the future.'));
      }
      // Fall-through.

    case UTAILLA_RESTRICTION_LEVEL_4:
      // Fall-through.
    case UTAILLA_RESTRICTION_LEVEL_2:
      if (empty($values['password']) && empty($first_restriction['password'])) {
        form_error($form['password'], t('A password must be entered.'));
      }
      break;
  }
}

/**
 * Helper to calculate the info to push to the DB.
 *
 * @param array $to_set
 *   An associative array with the values to set for the restriction.
 * @param array $first
 *   An associative array describing the restriction for the "first child"
 *   found (possibly the object proper, if applying to a media file object).
 *
 * @return array
 *   An associative array with the appropriate fields NULLed as per how each
 *   level should be handled.
 */
function utailla_calculate_restriction_level_info($to_set, $first) {
  $restriction = array(
    'level' => $to_set['level'],
    'password' => NULL,
    'hint' => NULL,
    'reason_option' => NULL,
    'custom_reason' => NULL,
    'end_time' => NULL,
  );

  if ($restriction['level'] > 0) {
    require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
    $restriction['password'] = (isset($to_set['password']) && !empty($to_set['password'])) ? user_hash_password($to_set['password']) : $first['password'];
    if (isset($to_set['hint'])) {
      $restriction['hint'] = $to_set['hint'];
    }
    $restriction['reason_option'] = $to_set['reason_option'];
    if (!empty($to_set['custom_reason'])) {
      $restriction['custom_reason'] = $to_set['custom_reason'];
    }
  }
  if (intval($restriction['level']) === UTAILLA_RESTRICTION_LEVEL_3) {
    $restriction['end_time'] = strtotime($to_set['end_time']);
  }

  return $restriction;
}

/**
 * Form submission handler; put the submitted values into effect.
 */
function utailla_access_level_form_submit(&$form, &$form_state) {
  list($object_in) = $form_state['build_info']['args'];
  $object = islandora_object_load($object_in->id);

  $schema = drupal_get_schema('utailla_media_file_restrictions');
  $restriction_info = array_intersect_key($form_state['values'], $schema['fields']);
  $first_restriction = $form_state['utailla_first_file_info'];
  $restriction_info = utailla_calculate_restriction_level_info($restriction_info, $first_restriction);

  $files = isset($form_state['values']['files']) ? array_filter($form_state['values']['files']) : array();
  $operand = in_array(UTAILLA_RESOURCE_CMODEL, $object->models) && !empty($files) ?
    array_map('islandora_object_load', array_keys(array_filter($files))) :
    $object->id;
  if ($operand == $object->id && !array_intersect(utailla_collection_like_models(), $object->models)) {
    $operand = array(islandora_object_load($operand));
  }

  $mod_path = drupal_get_path('module', 'utailla');
  $batch = array(
    'file' => "$mod_path/includes/access_levels.form.inc",
    'operations' => array(
      array('utailla_access_level_batch_op', array($restriction_info, $operand)),
    ),
  );
  batch_set($batch);
}

/**
 * Access level update batch operation.
 *
 * @param array $restriction_info
 *   An associative array containing the information to apply to each object.
 * @param string|array $objects
 *   Either a string indicating the root of a tree of objects over which to
 *   iterate, or an array of multiple PIDs.
 * @param array $context
 *   Drupal's batch context.
 */
function utailla_access_level_batch_op($restriction_info, $objects, &$context = NULL) {
  $sandbox =& $context['sandbox'];

  if (!isset($sandbox['iterator'])) {
    $sandbox['count'] = 0;
    if (is_array($objects)) {
      $sandbox['iterator'] = new ArrayIterator($objects);
    }
    else {
      $sandbox['iterator'] = new UtaillaFileTraversalIterator($objects, utailla_collection_like_models());
    }
    $sandbox['iterator']->rewind();
    if (!$sandbox['iterator']->valid()) {
      $context['message'] = t('Nothing to iterate.');
      return;
    }
  }
  else {
    $sandbox['iterator']->next();
    if (!$sandbox['iterator']->valid()) {
      $context['message'] = t('Done.');
      return;
    }
  }

  $current = $sandbox['iterator']->current();

  module_load_include('inc', 'utailla', 'includes/utilities');
  utailla_set_restriction($current, $restriction_info);

  $context['finished'] = $sandbox['count'] / ($sandbox['iterator'] instanceof Countable ?
    count($sandbox['iterator']) :
    $sandbox['count'] + 1);
  $sandbox['count']++;
}

/**
 * Helper function; list the models which get treated like collections.
 *
 * @return array
 *   An array of content models which should get treated like collections.
 */
function utailla_collection_like_models() {
  return array(
    'islandora:collectionCModel',
    UTAILLA_RESOURCE_CMODEL,
  );
}
