<?php
/**
 * @file
 * update media form and processing.
 */

 /**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function utailla_update_media(array $form, array &$form_state) 
{

    global $user;
    $form = array();
    $form['#tree'] = TRUE;

    $form['enteredBy'] = array(
        '#type' => 'textfield',
        '#title' => t('Entered By'),
        '#default_value' => $user->name,
        '#required' => TRUE,
    );

    $form['pid'] = array(
        '#type' => 'textfield',
        '#title' => t('Enter PID/Identifier (or range of PIDs) for asset(s)'),
        '#default_value' => '',
        '#required' => TRUE,
    	'#description' => t('ex. ailla:123456 or ailla:123456-ailla:123567'),
    );

    $form['update_type'] = array(
		'#type' => 'radios',
		'#title' => t('Update Type'),
		'#options' => array(
			t('Index Only'),
			t('Update Media Type (generates derivatives also)'),
			t('Generate Derivatives'),
		),
    	'#description' => t('test'),
    );

	
    $form['new_type'] = array(
		'#type' => 'radios',
        '#title' => t('New content type for asset(s)'),
		'#options' => array(
			t('PDF'),
			t('Audio'),
			t('Video'),
		),
    	'#description' => t('only works when starting value is binary'),
    	'#prefix' => 'Select type, if "Update Media Type" is the desired process',
    );

    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
    );
    
    return $form;

}

/**
 * Form submit.
 */
function utailla_update_media_submit($form, &$form_state) {
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'utailla', 'includes/update_media');

    global $user;
    $uname = $user->name;
	$updateType = $form_state['values']['update_type'] ;
	$pid = $form_state['values']['pid'] ;
	$enteredBy = $form_state['values']['enteredBy'] ;
	$new_type = $form_state['values']['new_type'] ;
		
	drupal_set_message('<pre>formState is: '.print_r($form_state['values'], true).'</pre>');
	drupal_set_message('<pre>updateType is: '.print_r($updateType, true).'</pre>');

	switch ($updateType) {
		case 0:
			$processUpdate = indexOnly($enteredBy, $pid) ;
			//drupal_set_message('<pre>processUpdate is: ' . print_r($processUpdate, true) . '</pre>');
			break;
		case 1:
			drupal_set_message('<pre>Processing changeType 1</pre>');
			$processUpdate = changeType($enteredBy, $pid, $new_type) ;
			drupal_set_message('<pre>Processing changeType 2</pre>');
			break;
		case 2:
			drupal_set_message('<pre>Processing updateDerivatives 1</pre>');
			$processUpdate = updateDerivatives($pid) ;
			drupal_set_message('<pre>Processing updateDerivatives 2</pre>');
			break;
	}
	
	//drupal_set_message('<pre>processUpdate is: '.print_r($processUpdate, true).'</pre>');
	
//	$queueB = queueBatch($form_state);
	$form_state['rebuild'] = TRUE;
}
