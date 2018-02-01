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

  $current_url = $_SERVER['REQUEST_URI'];
  $path = explode('/', $current_url);
  $lang_path = $path[1];

  // XXX: Reload the object to attempt to alleviate any form/caching issues.
  $object = islandora_object_load($object_in->id);

  $first_file = utailla_get_first_file($object);
  $info = (array) utailla_get_restriction($first_file->id) + array(
    'hint' => '',
    'level' => UTAILLA_RESTRICTION_LEVEL_1,
    'reason_option' => 0,
    'custom_reason' => '',
    'end_time' => NULL,
    'depositor_name' => '',
    'depositor_email' => '',
  );
  $form_state['utailla_first_file_info'] = $info;
  if ($lang_path == 'es') {
	$justifications = utailla_access_justification_options_spa();
  } else {
	$justifications = utailla_access_justification_options();
  }

  if (in_array(UTAILLA_RESOURCE_CMODEL, $object->models)) {
    $files = array();

    if (isset($form_state['values']['object_count'])) {
        if ($form_state['values']['object_count'] === 'all') {
          $results = islandora_basic_collection_get_member_objects($object_in, 0, 0);
          $object_count = reset($results);
        }
        else {
          $object_count = $form_state['values']['object_count'];
        }
    }
    else {
      $object_count = 20;
    }

    list($count, $members) = islandora_basic_collection_get_member_objects($object, 0, $object_count);

    foreach ($members as $member) {
      $member_info = utailla_get_restriction($member['object']['value']);
      $files[$member['object']['value']] = array(
        'title' => l($member['title']['value'], "islandora/object/{$member['object']['value']}/manage/utailla_access_levels"),
        'modified' => $member['date_modified']['value'],
        'level' => isset($member_info['level']) ? $member_info['level'] : t('Not set'),
        'hint' => isset($member_info['hint']) ? $member_info['hint'] : t('N/A'),
        'depositor_name' => isset($member_info['depositor_name']) ? $member_info['depositor_name'] : t('N/A'),
        'depositor_email' => isset($member_info['depositor_email']) ? $member_info['depositor_email'] : t('N/A'),
	  );
    }
 
     $form['object_count'] = array(
      '#type' => 'select',
      '#title' => t('File view count'),
      '#options' => array(
        20 => t('20'),
        50 => t('50'),
        100 => t('100'),
        250 => t('250'),
        500 => t('500'),
        'all' => t('All objects'),
      ),
      '#default_value' => 20,
      '#description' => t('How many file objects to be shown in the Files table. Defaults to 20.'),
      '#ajax' => array(
        'callback' => 'utailla_access_levels_object_count_callback',
        'wrapper' => 'files_table',
      ),
    );

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
      '#prefix' => '<div id="files_table">',
      '#suffix' => '</div>',
    );
  }

  $form['level'] = array(
    '#type' => 'radios',
    '#title' => t('Access Level'),
    '#options' => array(
      UTAILLA_RESTRICTION_LEVEL_1 => t('Level 0: Authenticated Access'),
      UTAILLA_RESTRICTION_LEVEL_2 => t('Level 1: Password'),
      UTAILLA_RESTRICTION_LEVEL_3 => t('Level 2: Time limit'),
      UTAILLA_RESTRICTION_LEVEL_4 => t('Level 3: Contact depositor'),
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

  $form['depositor_name'] = array(
    '#type' => 'textfield',
    '#maxlength' => 256,
    '#title' => t('Depositor Name'),
    '#default_value' => $info['depositor_name'],
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          'value' => UTAILLA_RESTRICTION_LEVEL_4,
        ),
      ),
    ),
  );

  $form['depositor_email'] = array(
    '#type' => 'textfield',
    '#maxlength' => 256,
    '#title' => t('Depositor Email'),
    '#default_value' => $info['depositor_email'],
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          'value' => UTAILLA_RESTRICTION_LEVEL_4,
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
 * Ajax callback to update the files table.
 */
function utailla_access_levels_object_count_callback(&$form, &$form_state) {
  return $form['files'];
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
      if (empty($values['password']) && empty($first_restriction['password'])) {
        form_error($form['password'], t('A password must be entered.'));
      }
      if (empty($values['end_time'])) {
        form_error($form['end_time'], t('An ending time must be selected.'));
      }
      elseif (strtotime($values['end_time']) < REQUEST_TIME) {
        form_error($form['end_time'], t('The ending time must be in the future.'));
      }
      break;

    case UTAILLA_RESTRICTION_LEVEL_4:
      if (empty($values['password']) && empty($first_restriction['password'])) {
        form_error($form['password'], t('A password must be entered.'));
      }
      if (empty($values['depositor_name'])) {
        form_error($form['depositor_name'], t('Depositor name must be entered.'));
      }
      if (empty($values['depositor_email'])) {
        form_error($form['depositor_email'], t('Depositor email must be entered.'));
      }
      break;

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
    'depositor_name' => NULL,
    'depositor_email' => NULL,
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
  if (intval($restriction['level']) === UTAILLA_RESTRICTION_LEVEL_4) {
      $restriction['depositor_name'] = $to_set['depositor_name'];
      $restriction['depositor_email'] = $to_set['depositor_email'];
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
//  drupal_set_message('<pre>'. print_r($schema['fields'], true) .'</pre>'); 
//  drupal_set_message('<pre>'. print_r($form_state['values'], true) .'</pre>'); 
  
  $restriction_info = array_intersect_key($form_state['values'], $schema['fields']);
  $first_restriction = $form_state['utailla_first_file_info'];
  $restriction_info = utailla_calculate_restriction_level_info($restriction_info, $first_restriction);
//  drupal_set_message('<pre>'. print_r($restriction_info, true) .'</pre>'); 

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
      $sandbox['iterator'] = new UtaillaArrayIterator($objects);
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

  $total = $sandbox['iterator'] instanceof Countable ?
    count($sandbox['iterator']) :
    $sandbox['count'] + 1;
  $context['finished'] = $sandbox['count'] / $total;
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
