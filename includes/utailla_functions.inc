<?php
/**
 * @file
 * Nightly Ingest functions.
 */

/**
 * 
 * deletes passwords from level 0 
 */

function setZero() 
{
	db_set_active();

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    	
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

 	$query = 'UPDATE utailla_media_file_restrictions SET end_time = :time, password = :pass, hint = :hint, reason_option = :ro, custom_reason = :cr WHERE level = :level';
 	$results = db_query($query, array(':level' => 0, ':time' => 0, ':pass' => NULL, ':cr' => NULL, ':hint' => NULL, ':ro' => NULL ));
 	$results = db_update('utailla_media_file_restrictions') ;
}

/**
 * 
 * gets list of level 0 restriction media files
 */

function getUnrestrictedOBJs() {
	echo "Hi from inside getUnrestrictedOBJs \n";
	db_set_active();

	module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
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

	$mediaInfo = array();
	$query = 'SELECT * FROM utailla_media_file_restrictions WHERE level=0';
	$results = db_query($query);

	if ($results->rowCount() != 0) {
		foreach ($results as $record) {
			$pid = $record->pid;
			$mediaInfo[$pid]['pid'] = $pid ;
			$mediaInfo[$pid]['level'] = $record->level;
		}
	}

	return $mediaInfo;

}


/**
 * Finds and returns MIME TYPE for each file in object.
 *
 * @param string $f
 * @return string
 */

function findMime($f)
{
    $m = "";
    $finfo = finfo_open(FILEINFO_MIME_TYPE); // return mime type ala mimetype extension
    $m = finfo_file($finfo, $f);
    finfo_close($finfo);
    return $m;
}

/**
 * updates the thumbnail based on the mimetype.
 *
 */

function replaceTN($pid, $type) {
	//echo "Hi from inside replaceTN \n";
    db_set_active();

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

	$object = islandora_object_load($pid);
	echo ("PID -- " . $object->id . "\n");
	
	switch ($type) {
		case 'audio':
			echo ("Working with audio" . PHP_EOL);
			$file = $path . 'images/audio.jpg' ;
			$mimetype = findMime($file) ;
			break;
		case 'video':
			echo ("Working with video" . PHP_EOL);
			$file = $path . 'images/video.png' ;
			$mimetype = findMime($file) ;
			break;
		case 'pdf':
			echo ("Working with pdf" . PHP_EOL);
			$file = $path . 'images/pdf.png' ;
			$mimetype = findMime($file) ;
			break;
		case 'image':
			echo ("Working with image " . $pid . PHP_EOL);
			$file = $path . 'images/image.png' ;
			$mimetype = "image/png" ;
			break;
		default:
			echo ("Working with other" . PHP_EOL);
			$file = $path . 'images/document.png' ;
			$mimetype = findMime($file) ;
			break;
	}

	reingestTN($pid, $file, $mimetype);
 	
}


/**
 * function to reingest thumbnail.
 *
*/
function reingestTN($pid, $file, $mimetype)
{
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

  	$tnFile = $file ;
  	print ("$tnFile is the file to ingest\n") ;
	
	$object = $fedoraConnection->repository->getObject($pid);
	
    $datastreams = $object->repository->api->a->listDatastreams($pid);
//	var_dump($datastreams);

	if (stristr($mimetype,'image')) {
	
		// Create/Update TN datastream
			islandora_run_derivatives($object, 'OBJ');
	
			$datastream = $object["TN"];
			$datastream->label = 'Thumbnail';
			$datastream->mimetype = $mimetype;
//  			$datastream_content = $datastream->content;
			
//			var_dump($datastream_content);
	} else {
	
		// Create/Update TN datastream
		if (!isset($object["TN"])) {
			$datastream_id = "TN";
			$controlGroup = "M";
			$datastream = $object->constructDatastream($datastream_id, $controlGroup);
			$datastream->label = 'Thumbnail';
			$datastream->mimetype = $mimetype;
			$datastream->setContentFromFile($tnFile);
			$object->ingestDatastream($datastream);
		} else {
			$datastream = $object["TN"];
			$datastream->label = 'Thumbnail';
			$datastream->mimetype = $mimetype;
			$datastream->content = $tnFile;
		}	
	
	}

	
}

/**
 * 
 * gets list of restricted media files
 */
function getRestrictedOBJs() {
	echo "Hi from inside getRestrictedOBJs \n";
	db_set_active();

	module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
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

	$resMedia = array();
	$query = 'SELECT pid FROM utailla_media_file_restrictions WHERE level>0';
	$results = db_query($query);

	if ($results->rowCount() != 0) {
		foreach ($results as $record) {
			$pid = $record->pid;
			$resMedia[] = $pid ;
		}
	}

	return $resMedia;

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

