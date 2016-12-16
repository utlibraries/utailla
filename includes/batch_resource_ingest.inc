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
	module_load_include('inc', 'islandora', 'includes/derivatives');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    $namespace = $config['default_namespace'];

    $params = array();
    setup($config, $params);
    $fedoraConnection = $params['fedoraConnection'];
 
    $username = "fedoraAdmin";
    $now = date("m/d/Y h:i a");

	// 	get new pid	
	$newPid = $fedoraConnection->apim->getNextPid($namespace, 1);
     
	$resource['identifier'] = $newPid ;
	echo ("resPid == " . $resource['identifier'] . "\n") ;
	$content = utailla_build_minimal_resource_mods($resource);
	
	$object = $fedoraConnection->repository->constructObject($resource['identifier']);
	echo ("PID -- " . $object->id . "\n");

	$resourceOut = "Resource:  " . $resource['title'] . " has PID:  " . $object->id . PHP_EOL;
	
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
	utailla_set_default_thumb($object);
	$restriction=utailla_add_restriction_if_missing($object);
	islandora_set_defer_derivatives_flag($object);
	 
	 $mediaOut = ingestMedia($fedoraConnection, $config, $resource);
	 $resourceOut .= PHP_EOL . $mediaOut . PHP_EOL;
	 return $resourceOut;
}

/**
 * Batch ingest of resource media files.
 */
function ingestMedia($fedoraConnection, $config, $resource) 
{
	module_load_include('inc', 'utailla', 'includes/utilities');
	module_load_include('inc', 'utailla', 'includes/db');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');
	module_load_include('inc', 'islandora', 'includes/derivatives');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    $namespace = $config['default_namespace'];

    $params = array();
    setup($config, $params);
 
    $username = "fedoraAdmin";
    $now = date("m/d/Y h:i a");

	foreach ($resource as $key => $value) {
		echo "$key === $value\n";
	}

	$mediaPath = $resource['ingest_path'] . "/" ;
	$resMedia = scandir($mediaPath);
	$resource['mediaPids'] = array();
	$mediaOut = "" ;
	foreach ($resMedia as $key => $value) {
		if ($value == '.' || $value == '..' || $value == 'Thumbs.db' || $value == '.DS_Store') { continue;}
		echo "$key === $value\n";
		$path_parts = pathinfo($value);
		$media['resPid'] = $resource['identifier'];
		$media['title'] = $value ;
		$media['path'] = $mediaPath . $value;
		$media['mime'] = mime_content_type($media['path']);
		$media['filename'] = $path_parts['filename'];
		$media['enteredBy'] = $username ;
		
		if (!file_exists($media['path'])) { 
			$mediaOut .= "Batch " . $value . " is not a valid media file\n";
			continue;			
		}
				
		// 	get new pid	
		$newPid = $fedoraConnection->apim->getNextPid($namespace, 1);
	 	$resource['mediaPids'][] = $newPid ;
		$media['identifier'] = $newPid ;
		$content = utailla_build_minimal_media_mods($media);
	
		$object = $fedoraConnection->repository->constructObject($media['identifier']);
		echo ("PID -- " . $object->id . "\n");
		
		$modelInfo = getMediaModels($media) ;
		$media['typeOfResource'] = $modelInfo['type'] ;
		$content = utailla_build_minimal_media_mods($media);
		$mediaOut .= "Media:  " . $media['title'] . " (Type: " . $modelInfo['type'] . ") has PID: " . $object->id . PHP_EOL;
		
		var_dump($media);
		
		$object->models = $modelInfo['model'];
    
	  	// 	Add all collection relationships.
   		$object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $media['resPid']);
    	$object->label = $media['title'];
    	$object->owner = $username; 
 
	  	$mods_ds = $object->constructDatastream('MODS', 'M');
	  	$mods_ds->label = 'MODS Record';
	  	$mods_ds->mimetype = 'application/xml';
	  	$mods_ds->content = $content ;
	  	$object->ingestDatastream($mods_ds);

	  	$obj_ds = $object->constructDatastream('OBJ', 'M');
	  	$obj_ds->label = basename($media['path']);
		$obj_ds->checksumType = 'MD5';
	  	$obj_ds->mimetype = $media['mime'];
	  	$obj_ds->setContentFromFile($media['path']);
	  	$object->ingestDatastream($obj_ds);

	    islandora_add_object($object);

		utailla_set_restriction($object, array('level' => UTAILLA_RESTRICTION_LEVEL_2));
		insertPassword($object);
//		utailla_set_default_thumb($object);
		utailla_set_restricted_thumb($object);
		if ($modelsInfo['type'] != 'text' && $modelsInfo['type'] != 'binary' ) { 
			echo ("Running Derivatives for of Type " . $modelsInfo['type'] . " and PID " . $object->id . "\n");
			islandora_run_derivatives($object, 'OBJ'); 
		}
	}

	var_dump($resource['mediaPids']);
	return $mediaOut;

}

/**
 * Ingests the default "restricted" thumbnail on an object.
 */
function utailla_set_restricted_thumb(AbstractObject $object) {
	$path = drupal_get_path('module', 'utailla');
	$datastream = $object->constructDatastream('TN', 'M');
	$datastream->label = 'Restricted Thumbnail';
	$datastream->mimetype = 'image/png';
	$datastream->setContentFromFile("$path/images/restricted.png", FALSE);
	$object->ingestDatastream($datastream);
}


/**
 * Build minimal MODS datastream for Resource
 */
function utailla_build_minimal_resource_mods($resource) 
{
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
// 	if ($resource['colPid']) {
// 		$pid = $resource['colPid'];
// 		$collection = islandora_object_load($pid);
// 		if ($collection) {
// 			$writer->startElement('relatedItem');
// 			$writer->writeAttribute('type', 'host');
// 			$writer->startElement('titleInfo');
// 			$writer->writeAttribute('displayLabel', 'Collection name');
// 			$writer->startElement('title');
// 			$writer->writeAttribute('authority', 'aillaCollection');
// 			$writer->writeAttribute('valueURI', utailla_get_uri_from_pid($pid));
// 			$writer->text($collection->label);
// 			$writer->endElement();
// 			$writer->endElement();
// 			$writer->endElement();
// 		}
// 	}
// 
  // Date Created.
	if ($resource['date']) {
		$writer->startElement('originInfo');
		$writer->writeAttribute('eventType', 'production');
		$writer->startElement('dateCreated');
		$writer->text($resource['date']);
		$writer->endElement();
		$writer->endElement();
	}

  // Type of Resource.
	$writer->startElement('typeOfResource');
	$writer->text('mixed material');
	$writer->endElement();

  // identifier.
	if ($resource['identifier']) {
		$resourceURI = utailla_get_uri_from_pid($resource['identifier']);

		$writer->startElement('identifier');
		$writer->writeAttribute('type', 'URI');
		$writer->text($resourceURI);
		$writer->endElement();
	}

  $writer->endElement();
  $writer->endDocument();
  return $writer->flush();

}

/**
 * Build minimal MODS datastream for Media
 */
function utailla_build_minimal_media_mods($media) 
{
	module_load_include('inc', 'utailla', 'includes/utilities');

	$writer = utailla_get_mods_writer();

  // enteredBy.
	if ($media['enteredBy']) {
		$writer->startElement('note');
		$writer->writeAttribute('type', 'record producer');
		$writer->text($media['enteredBy']);
		$writer->endElement();
	}

  // Date Created.
	if ($media['date']) {
		$writer->startElement('originInfo');
		$writer->writeAttribute('eventType', 'production');
		$writer->startElement('dateCreated');
		$writer->writeAttribute('keyDate', 'yes');
		$writer->writeAttribute('displayLabel', 'Date research material was created');
		$writer->text($media['date']);
		$writer->endElement();
		$writer->endElement();
	}

  // Date Archived.
	$objDateTime = new DateTime('NOW');
	$dateA = $objDateTime->format('c'); // ISO8601 formated datetime

	$writer->startElement('originInfo');
	$writer->writeAttribute('eventType', 'provenance');
	$writer->startElement('dateModified');
	$writer->endElement();
	$writer->startElement('dateOther');
	$writer->writeAttribute('displayLabel', 'Date archived');
	$writer->text($dateA);
	$writer->endElement();
	$writer->endElement();

  // Type of Resource.
	if ($media['typeOfResource']) {
		$writer->startElement('typeOfResource');
		$writer->text($media['typeOfResource']);
		$writer->endElement();
	}

  // Genres.
	$writer->startElement('genre');
	$writer->writeAttribute('authority', 'aillaMediaGenres');
	$writer->writeAttribute('lang', 'eng');
	$writer->writeAttribute('displayLabel', 'Content type (English)');
	$writer->text('primary text');
	$writer->endElement();

	$writer->startElement('genre');
	$writer->writeAttribute('authority', 'aillaMediaGenres');
	$writer->writeAttribute('lang', 'spa');
	$writer->writeAttribute('displayLabel', 'Content type (Spanish)');
	$writer->text('texto primario');
	$writer->endElement();

  // identifier.
	if ($media['identifier']) {
		$mediaURI = utailla_get_uri_from_pid($media['identifier']);

		$writer->startElement('identifier');
		$writer->writeAttribute('type', 'URI');
		$writer->text($mediaURI);
		$writer->endElement();
	}
	
  $writer->endElement();
  $writer->endDocument();
  return $writer->flush();

}

/**
 * function to setup the environment, e.g. a connecction to Fedora, etc.
*/
function setup($config, &$params) 
{
  // Fedora information
  $fedoraHost = $config['fedora_host'];
  $fedoraUsr  = $config['fedora_usr'];
  $fedoraPwd  = $config['fedora_pwd'];

  // create a connection to Fedora
  $fedoraConnection = new FedoraConnection($fedoraHost, $fedoraUsr, $fedoraPwd);
  $params['fedoraConnection'] = $fedoraConnection;

}

function getMediaModels($media)
{
	//got this from the dgi migrate functions
	$mediaModels = array(
		1 => 'islandora:sp-audioCModel',
		2 => 'islandora:sp-audioCModel',
		3 => 'islandora:sp-audioCModel',
		4 => 'islandora:sp_pdf',
		5 => 'islandora:sp_document',
		6 => 'islandora:sp_document',
		7 => 'islandora:sp_document',
		8 => 'islandora:sp_document',
		9 => 'islandora:sp_basic_image',
		10 => 'islandora:sp_document',
		11 => 'islandora:sp_large_image_cmodel',
		12 => 'islandora:binaryObjectCModel',
		13 => 'islandora:sp_videoCModel',
		14 => 'islandora:sp_videoCModel',
		15 => 'islandora:sp_videoCModel',
		16 => 'islandora:sp_document',
		17 => 'islandora:sp_videoCModel',
		18 => 'islandora:binaryObjectCModel',
		19 => 'islandora:binaryObjectCModel',
		20 => 'islandora:sp_videoCModel',
		21 => 'islandora:sp_videoCModel',
		22 => 'islandora:sp_videoCModel',
		23 => 'islandora:sp_videoCModel',
		24 => 'islandora:sp_large_image_cmodel',
		25 => 'islandora:binaryObjectCModel',
//		26 => 'islandora:binaryObjectCModel',
		27 => 'islandora:sp_basic_image',
		28 => 'islandora:sp_videoCModel',
		29 => 'islandora:sp_videoCModel',
		30 => 'islandora:sp-audioCModel',
	);
	$mimes = array(
		1 => 'audio/mp3',
		2 => 'audio/wav',
		3 => 'audio/mpeg',
		4 => 'application/pdf',
		5 => 'application/msword',
		6 => 'text/html',
		7 => 'text/plain',
		8 => 'application/Wordperfect5.1',
		9 => 'image/jpeg',
		10 => 'text/richtext',
		11 => 'image/tiff',
		12 => 'text/xml',
		13 => 'video/mj2',
		14 => 'video/avi',
		15 => 'video/mpeg',
		16 => 'application/vnd.oasis.opendocument',
		17 => 'video/quicktime',
		18 => 'text/x-eaf+xml',
		19 => 'text/x-trs+xml',
		20 => 'video/vnd.rn-realvideo',
		21 => 'video/x-ms-wm',
		22 => 'video/mpeg',
		23 => 'video/mp4',
		24 => 'image/tiff',
		25 => 'text/xml',
//		26 => 'application/vnd.openxmlformats-officedocument.wordprocessingml.document',
		27 => 'image/png',
		28 => 'video/mp4',
		29 => 'video/m4v',
		30 => 'audio/x-wav',
	);
	$metaMap = array(
	    'text' => array(4, 5, 6, 7, 8, 10, 12, 16, 18, 19, 25),
		'audio' => array(1, 2, 3, 30),
		'image' => array(9, 11, 24, 27),
		'video' => array(13, 14, 15, 17, 20, 21, 22, 23, 28, 29),
	);
	
	if (in_array($media['mime'], $mimes)) {
		$key = array_search($media['mime'], $mimes);
		$modelsInfo['model'] =  $mediaModels[$key];
		if (in_array($key, $metaMap['text'])) { $modelsInfo['type'] = 'text' ; }  
		if (in_array($key, $metaMap['audio'])) { $modelsInfo['type'] = 'sound recording' ; }  
		if (in_array($key, $metaMap['video'])) { $modelsInfo['type'] = 'moving image' ; }  
		if (in_array($key, $metaMap['image'])) { $modelsInfo['type'] = 'still image' ; }  
	} else {
		$modelsInfo['model'] =  "islandora:binaryObjectCModel" ;
		$modelsInfo['type'] = 'binary' ;
	}
		
	return $modelsInfo;
	
}

function queueBatch(&$formFields)
{
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';
        
    $collection = $formFields['values']['collection'];
    $ingest_path = $formFields['values']['ingest_path'];
    $batch_date = $formFields['values']['batch_date'];
    $enteredBy = $formFields['values']['enteredBy'];
    $numResources = (int) $formFields['values']['numResources'];
    $completed = 0;

	drupal_set_message('Form Processed Successful<br>');
	drupal_set_message('Collection: === ' . $collection . '<br>');
	drupal_set_message('Path: === ' . $ingest_path . '<br>');
	drupal_set_message('Entered By: === ' . $enteredBy . '<br>');
	drupal_set_message('Number of Resources in Batch: === ' . $numResources . '<br>');
	drupal_set_message('Batch ingest date: === ' . $batch_date . '<br>');
	
	$query="INSERT INTO {utailla_batch_ingest_queue} (collection, ingest_path, batch_date, enteredBy, numResources, completed) VALUES ('$collection', '$ingest_path', '$batch_date', '$enteredBy', $numResources, $completed)" ;
	$result = db_query($query);

}

function insertPassword($object) 
{

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
	$level = 1;
	$hint = $config['ingestHint'];
	$password = $config['ingestPass'] ;
	$pid = $object->id;
	$option = 8;
	$reason = 'Administrative curation in progress';
	
    // Database information
    $dbDriver  = $config['database']['driver'];
    $dbHost = $config['database']['host'];
    $dbUsr  = $config['database']['usr'];
    $dbPwd  = $config['database']['pwd'];
    $dbName  = $config['database']['name'];

    // connect to database
    $utailla_database = array(
        'database' => $dbName,
        'username' => $dbUsr,
        'password' => $dbPwd,
        'host' => $dbHost,
        'driver' => $dbDriver,
    );

    Database::addConnectionInfo('UTAILLA', 'default', $utailla_database);
    db_set_active('UTAILLA');

	$query = 'UPDATE utailla_media_file_restrictions SET level = :level, hint = :hint, password = :password, reason_option = :option, custom_reason = :reason WHERE pid = :pid';
	$results = db_query($query, array(':level' => $level, ':hint' => $hint, ':password' => $password, ':pid' => $pid, ':option' => $option, ':reason' => $reason));
	$results = db_update('utailla_media_file_restrictions') ;

	
}
