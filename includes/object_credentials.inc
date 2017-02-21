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
		$hintTitle = 'Insinuación';
		$submitTitle = 'Autenticar';
		$reasonTitle = 'Razón';
	} else {
		$hintTitle = 'Hint';
		$submitTitle = 'Authenticate';
		$reasonTitle = 'Reason';
	}

  $restriction = utailla_get_restriction($object->id);
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
    $reasons = utailla_access_justification_options();
    $reason = isset($reasons[$restriction['reason_option'] + 1]) ? $reasons[$restriction['reason_option']] : $restriction['custom_reason'];
    $form['reason'] = array(
      '#type' => 'item',
      '#title' => t($reasonTitle),
      '#description' => check_plain($reason),
    );
  }
  if ($restriction['level'] == UTAILLA_RESTRICTION_LEVEL_4) {
    // @XXX This will get silly.
    $parents = islandora_basic_collection_get_parent_pids($object);
    foreach ($parents as $parent) {
      $parent = islandora_object_load($parent);
      if (isset($parent['MODS']) && in_array(UTAILLA_RESOURCE_CMODEL, $parent->models)) {
        $mods_doc = new DOMDocument();
        $mods_doc->loadXML($parent['MODS']->content);
        $mods_xpath = new DOMXPath($mods_doc);
        $mods_xpath->registerNamespace('mods', 'http://www.loc.gov/mods/v3');
        $xpath_results = $mods_xpath->query('/mods:mods/mods:name/mods:namePart');
        if ($xpath_results->length && $xpath_results->item(0)->nodeValue) {
          $name = trim($xpath_results->item(0)->nodeValue);
          $content_model = UTAILLA_CONTRIBUTOR_CMODEL;
          $query = <<<EOQ
            PREFIX islandora: <http://islandora.ca/ontology/relsext#>
            SELECT ?user
            FROM <#ri>
            WHERE {
              {
                ?pid <fedora-model:label> "$name";
                <fedora-model:hasModel> <info:fedora/$content_model>;
                <islandora:hasAssociatedUser> ?user
              }
              UNION {
                ?pid <fedora-model:label> "$name";
                <fedora-model:hasModel> <info:fedora/$content_model>;
                <islandora:hasAssociatedContact> ?user
              }
            }
EOQ;
          $connection = islandora_get_tuque_connection();
          $results = $parent->repository->ri->sparqlQuery($query, 1);
          foreach ($results as $result) {
            $user_uri = $result['user']['value'];
            $uid = substr($user_uri, strrpos($user_uri, '/') + 1, strlen($user_uri) - 1);
            $contact = user_load($uid);
            if ($contact) {
              $contact_info = "$contact->name: $contact->mail";
              break 2;
            }
          }
        }
      }
    }
    if (isset($contact_info)) {
      $form['contact_info'] = array(
        '#type' => 'item',
        '#title' => t('Contact'),
        '#description' => check_plain($contact_info),
      );
    }
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
