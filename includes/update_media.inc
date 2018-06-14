<?php
/**
 * @file
 * Performs updates to media files 
 * updates content type
 * regenerates derivatives
 * re-index/force indexes to solr
 */


/**
 * Runs indexing on submitted PIDs.
 */
function indexOnly($enteredBy, $pid) {
   module_load_include('php', 'islandora_solr', 'SolrPhpClient/Apache/Solr/Service');

 	$range = preg_replace('/ailla:/', '', $pid);
 	$pieces = explode("-", $range);
 	foreach (range($pieces[0], $pieces[1]) as $n) {
 		$buildPid = 'ailla:' . $n ;
		//echo $buildPid . PHP_EOL;
		indexMe($buildPid) ;
	}

}

/**
 * changes content type of Object away from binary
 */
function changeType($enteredBy, $pid, $new_type) {
   module_load_include('php', 'islandora_solr', 'SolrPhpClient/Apache/Solr/Service');

 	$range = preg_replace('/ailla:/', '', $pid);
 	$pieces = explode("-", $range);
 	if (!(isset($pieces[1]))) { $pieces[1] = $pieces[0] ; }
 	foreach (range($pieces[0], $pieces[1]) as $n) {
 		$buildPid = 'ailla:' . $n ;
		//echo $buildPid . PHP_EOL;
		drupal_set_message('<pre>buildPid is: ' . print_r($buildPid, true) . '</pre>');
		$fixed = fixType($buildPid, $new_type) ;
		drupal_set_message('<pre>fixed is: ' . print_r($fixed, true) . '</pre>');

	}

}

/**
 * function to fix mediaType content model.
 *
*/
function fixType($pid, $new_type) {

	module_load_include('inc', 'islandora', 'includes/derivatives');
	module_load_include('inc', 'islandora', 'includes/regenerate_derivatives.form');
	module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');
	module_load_include('inc', 'utailla', 'includes/utailla_functions');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    $namespace = $config['default_namespace'];
    
    $params = array();
    setup($config, $params);
    $fedoraConnection = $params['fedoraConnection'];
 
    $username = "fedoraAdmin";
    $now = date("m/d/Y h:i a");
        
	$object = islandora_object_load($pid);
	$mediaOut = "" ;
	
	//var_dump($object) ;
	
	if (!$object) {
		$mediaOut =  ("PID -- " . $pid . " did not load\n");
		drupal_set_message('<pre>' . print_r($mediaOut, true) . '</pre>');
	 	return $mediaOut;
	} else {
 		$mediaOut =  ("PID -- " . $pid . " processed\n");
		drupal_set_message('<pre>Object load successful</pre>');	
	}

	switch ($new_type) {
		case 0:
			$contentType = "islandora:sp_pdf" ;
			$type = "pdf";
			break;
		case 1:
			$contentType = "islandora:sp-audioCModel" ;
			$type = "audio";
			break;
		case 2:
			$contentType = "islandora:sp_videoCModel" ;
			$type = "video";
			break;
	}
		
	$object->relationships->remove(FEDORA_MODEL_URI, 'hasModel', 'islandora:binaryObjectCModel');
	$object->relationships->add(FEDORA_MODEL_URI, 'hasModel', $contentType);
	islandora_run_derivatives($object, 'OBJ'); 

 	$mediaOut .=  ("type -- " . $type . " processed\n");
 	return $mediaOut;
}

/**
 * function to process derivatives.
 *
*/
function updateDerivatives($pid) {

	module_load_include('inc', 'islandora', 'includes/derivatives');
	module_load_include('inc', 'islandora', 'includes/regenerate_derivatives.form');
	module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');
	module_load_include('inc', 'utailla', 'includes/utailla_functions');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    $namespace = $config['default_namespace'];
    
    $params = array();
    setup($config, $params);
    $fedoraConnection = $params['fedoraConnection'];
 
    $username = "fedoraAdmin";
    $now = date("m/d/Y h:i a");
    
	$object = islandora_object_load($pid);
	$mediaOut = "" ;
	
	//var_dump($object) ;
	
	if (!$object) {
		$mediaOut =  ("PID -- " . $pid . " did not load\n");
		drupal_set_message('<pre>' . print_r($mediaOut, true) . '</pre>');
 		return $mediaOut;
	} else {
 		$mediaOut =  ("PID -- " . $pid . " processed\n");
		drupal_set_message('<pre>Object load successful</pre>');	
	}

	islandora_run_derivatives($object, 'OBJ'); 

 	return $mediaOut;
        
}

/**
 * function to index PID.
 *
*/
function indexMe($pid) {

    require_once '/var/www/html/sites/all/libraries/tuque/RepositoryConnection.php';
    require_once '/var/www/html/sites/all/libraries/tuque/Repository.php';
    require_once '/var/www/html/sites/all/libraries/tuque/FedoraApi.php';
    require_once '/var/www/html/sites/all/libraries/tuque/FedoraApiSerializer.php';
    require_once '/var/www/html/sites/all/libraries/tuque/Cache.php';

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);

    $pwd = $config['fedora_pwd'];
    $usr = $config['fedora_usr'];
    
    $connection = new RepositoryConnection();
    $repository = new FedoraRepository(new FedoraApi($connection), new SimpleCache());

    $curl = new CurlConnection();
    $curl->username = $usr;
    $curl->password = $pwd;

        //var_dump($pid);
        $response = $curl->getRequest('http://127.0.0.1:8080/fedoragsearch/rest'
            . '?operation=updateIndex'
            . '&action=fromPid'
            . '&restXslt=copyXml'
            . "&value=$pid"
        );
        
}

