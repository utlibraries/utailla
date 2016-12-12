<?php

/**
 * @file
 * Instigates the batch submission of resources to Fedora.
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
function utailla_batch_resource(array $form, array &$form_state) 
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

    $form['collection'] = array(
        '#type' => 'textfield',
        '#title' => t('Enter PID/Identifier of parent collection (ex. ailla:123456)'),
        '#default_value' => '',
        '#required' => TRUE,
    );

    $form['ingest_path'] = array(
        '#type' => 'textfield',
        '#title' => t('Enter folder name'),
        '#default_value' => '',
        '#required' => TRUE,
    );

    $form['batch_date'] = array(
        '#type' => 'textfield',
        '#title' => t('Batch Date'),
        '#default_value' => '',
        '#required' => TRUE,
    );

    $form['numResources'] = array(
        '#type' => 'textfield',
        '#title' => t('Enter enter number of resources in this batch (max. 500)'),
        '#default_value' => '',
        '#required' => TRUE,
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
function utailla_batch_resource_submit($form, &$form_state) {
    /* fedora connection functions live in the islandora module */
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');

    global $user;
    $uname = $user->name;

//	drupal_set_message('<pre>formState is: '.print_r($form_state['values'], true).'</pre>');
	$queueB = queueBatch($form_state);
	$form_state['rebuild'] = TRUE;
}

