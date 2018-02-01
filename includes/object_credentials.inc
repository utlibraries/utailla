<?php
/**
 * @file
 * Object credential code.
 */

/**
 * Object credentials form.
 */
function utailla_object_credentials_form($form, &$form_state, $object) {
	form_load_include($form_state, 'inc', 'utailla', 'includes/object_credentials');
	form_load_include($form_state, 'inc', 'utailla', 'includes/db');
	form_load_include($form_state, 'inc', 'utailla', 'includes/utilities');
	form_load_include($form_state, 'inc', 'islandora_basic_collection', 'includes/utilities');

	$current_url = $_SERVER['REQUEST_URI'];
	$path = explode('/', $current_url);
	$lang_path = $path[1];

	$lang_path = $path[1];
	if ($lang_path == 'es') {
		$hintTitle = 'Pista';
		$submitTitle = 'Autenticar';
		$reasonTitle = 'Razón';
	} else {
		$hintTitle = 'Hint';
		$submitTitle = 'Authenticate';
		$reasonTitle = 'Reason';
	}

  $restriction = utailla_get_restriction($object->id);
  $msg = "";
  if ($restriction['level'] == UTAILLA_RESTRICTION_LEVEL_4) {
	if ( ($restriction['depositor_name'] != NULL) && ($restriction['depositor_name'] != "") ) {
      if ($lang_path == 'es') {
		$msg .= "Contactar a " . $restriction['depositor_name'] ;
      } else {
		$msg .= "Contact " . $restriction['depositor_name'] ;      
	  }
	}
	if ( ($restriction['depositor_email'] != NULL) && ($restriction['depositor_email'] != "") ) {
      if ($lang_path == 'es') {
		$msg .= " (" . $restriction['depositor_email'] . ") para acceder.";
      } else {
	    $msg .= " (" . $restriction['depositor_email'] . ") for access.";
      }
	} 
  }

  $form['cutomtext'] = array(
	'#type' => 'item',
	'#description' => check_plain($msg),
  );
        
  $form['object_pass'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#size' => 40,
  );
  $form['object'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t($submitTitle),
  );
  if (isset($restriction['hint']) && $restriction['hint']) {
    $form['hint'] = array(
      '#type' => 'item',
      '#title' => t($hintTitle),
      '#description' => check_plain($restriction['hint']),
    );
  }
  if (isset($restriction['reason_option']) && !is_null($restriction['reason_option'])) {
      if ($lang_path == 'es') {
		$reasons = utailla_access_justification_options_spa();
      } else {
	    $reasons = utailla_access_justification_options();
      }

    $reason = isset($reasons[$restriction['reason_option'] + 1]) ? $reasons[$restriction['reason_option']] : $restriction['custom_reason'];
    $form['reason'] = array(
      '#type' => 'item',
      '#title' => t($reasonTitle),
      '#description' => check_plain($reason),
    );
  }

  
  return $form;
}

/**
 * Object credentials form submit.
 */
function utailla_object_credentials_form_submit($form, &$form_state) {
  require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
  module_load_include('inc', 'utailla', 'includes/db');
  $pid = $form_state['values']['object'];
  $restriction = utailla_get_restriction($pid);
  if (isset($restriction['password']) && $restriction['password']) {
    if (_password_crypt('sha512', $form_state['values']['object_pass'], $restriction['password']) === $restriction['password']) {
      utailla_enable_user_media_file_authorization($pid);
    }
  }
}
