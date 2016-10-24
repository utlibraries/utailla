<?php
/**
 * @file
 * Nightly Ingest functions.
 */

/**
 * Batch ingest of resources.
 */

function utailla_ingest_resource($resource) {
	module_load_include('inc', 'utailla', 'includes/utilities');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    $namespace = $config['default_namespace'];

    $params = array();
    setup($config, $params);
    $fedoraConnection = $params['fedoraConnection'];
 
    $username = "fedoraAdmin";
    $now = date("m/d/Y h:i a");

//     $my_islandora_tuque = new IslandoraTuque();
//     $repository = $my_islandora_tuque->repository;

//		$repository = new IslandoraTuque();

// 		$resource['title'] = $value;
// 		$resource['colPid'] = $batchRow['colPid'];
// 		$resource['date'] = $batchRow['date'];
// 		$resource['path'] = $batchRow['path'] . $value;

	foreach ($resource as $key => $value) {
		echo "$key === $value\n";
	}

	$content = utailla_build_mods($resource);
	
	var_dump($content);
	
	// 	get new pid	
     $newPid = $fedoraConnection->apim->getNextPid($namespace, 1);
     
 	$resource['resPid'] = $newPid ;
 	echo ("resPid == " . $resource['resPid'] . "\n") ;

    $object = $fedoraConnection->repository->constructObject($resource['resPid']);
	
	echo ("PID -- " . $object->id . "\n");
  	$object->models = array(UTAILLA_RESOURCE_CMODEL);
   
 	// 	Add all collection relationships.
  	$object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $resource['colPid']);
   	$object->label = $resource['title'];
   	$object->owner = $username; 

 	$mods_ds = $object->constructDatastream('MODS', 'M');
 	$mods_ds->label = 'MODS Record';
 	$mods_ds->mimetype = 'application/xml';
 	$mods_ds->content = $content ;
 	$object->ingestDatastream($mods_ds);
  
     // Add media file collection policy.
     utailla_set_collection_policy($object, 'media_collection_policy.xml');
     islandora_add_object($object);
	 $restriction=utailla_add_restriction_if_missing($object);
}

function utailla_build_mods($resource) {
	module_load_include('inc', 'utailla', 'includes/utilities');

	$writer = utailla_get_mods_writer();

  // Titles.
	if ($resource['title']) {

		$writer->startElement('titleInfo');
		$writer->writeAttribute('displayLabel', 'Title (English)');
		$writer->writeAttribute('lang', 'eng');
		$writer->startElement('title');
		$writer->text($resource['title']);
		$writer->endElement();
		$writer->endElement();
	}
	
  // Collection Name.
	if ($resource['colPid']) {
		$pid = $resource['colPid'];
		$collection = islandora_object_load($pid);
		if ($collection) {
			$writer->startElement('relatedItem');
			$writer->writeAttribute('type', 'host');
			$writer->startElement('titleInfo');
			$writer->writeAttribute('displayLabel', 'Collection name');
			$writer->startElement('title');
			$writer->writeAttribute('authority', 'aillaCollection');
			$writer->writeAttribute('valueURI', utailla_get_uri_from_pid($pid));
			$writer->text($collection->label);
			$writer->endElement();
			$writer->endElement();
			$writer->endElement();
		}
	}

  // Date Created.
	if ($resource['date']) {
		$writer->startElement('originInfo');
		$writer->writeAttribute('eventType', 'production');
		$writer->startElement('dateCreated');
		$writer->text($resource['date']);
		$writer->endElement();
		$writer->endElement();
	}


  $writer->endElement();
  $writer->endDocument();
  return $writer->flush();

}

/**
 * function to setup the environment, e.g. a connecction to Fedora, etc.
*/
function setup($config, &$params) {
	echo "in setup\n";
  // Fedora information
  $fedoraHost = $config['fedora_host'];
  $fedoraUsr  = $config['fedora_usr'];
  $fedoraPwd  = $config['fedora_pwd'];

  // create a connection to Fedora
  // echo 'host: ' . $fedoraHost . ' usr: ' . $fedoraUsr . ' pwd: ' . $fedoraPwd . PHP_EOL;
  $fedoraConnection = new FedoraConnection($fedoraHost, $fedoraUsr, $fedoraPwd);
  $params['fedoraConnection'] = $fedoraConnection;

}
