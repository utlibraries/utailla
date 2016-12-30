<?php

/**
 * @file
 * Allows for a downloadable link for OBJs.
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
function utailla_view_download(array $form, array &$form_state) 
{

    module_load_include('php', 'islandora_solr', 'SolrPhpClient/Apache/Solr/Service');

	$path = current_path();
	$path = explode('/', $path);
	$page = $path[1];
	$pidnum = array_pop($path);
		
    $form = array();
    $form['#tree'] = TRUE;

	$form['pid'] = array(
        '#type' => 'textfield',
        '#title' => t('PID'),
        '#default_value' => $pidnum,
        '#disabled' => TRUE,
    );

    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Download Media'),
    );
    
    return $form;

}


/**
 * Form submit.
 */
function utailla_batch_resource_submit($form, &$form_state) {
    /* fedora connection functions live in the islandora module */
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');

//islandora-ailla-dev.lib.utexas.edu/islandora/object/ailla%3A245314
//http://islandora-ailla-dev.lib.utexas.edu/islandora/object/ailla%3A245314/datastream/OBJ/download
//http://islandora-ailla-dev.lib.utexas.edu/islandora/object/ailla%3A245314/datastream/JPG/download

	$current_url = 'http://' .$_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI'];

	$form_state['redirect'] = drupal_goto($path = $current_url . '/datastream/OBJ/download';
}

