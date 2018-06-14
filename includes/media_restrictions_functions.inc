<?php
/**
 * @file
 * Nightly Ingest functions.
 */

/**
 * Step 1 in restoring file restrictions.
 * creates array of all media objects in fedora
 */

function findFedoraOBJs() {
	//this step occurs on QA/PROD
	echo "Hi from inside findFedoraOBJs : Step 1 in restoring file restrictions \n";
	db_set_active();

	module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');
//    module_load_include('php', 'islandora_solr', 'SolrPhpClient/Apache/Solr/Service');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

//     $params = array();
//     setup($config, $params);
//     $fedoraConnection = $params['fedoraConnection'];
     
    //find objects with the OBJ datastream
	
	$mediaOBJ = array();
	
	$query = <<<EOQ
	SELECT ?pid
	FROM <#ri>
	WHERE {
	 ?dss <fedora-view:disseminationType> <info:fedora/*/OBJ> .
	 ?pid <info:fedora/fedora-system:def/view#disseminates>
	 ?dss
	}
EOQ;

	// Connect to Tuque and grab the results.
	$connection = islandora_get_tuque_connection();
	$results = $connection->repository->ri->sparqlQuery($query, 'unlimited');
	
	foreach ($results as $op) {
	    $mediaOBJ[] =  $op['pid']['value'] . "\n";
    }

	return $mediaOBJ;

}

/**
 * Step 2 in restoring file restrictions.
 * creates file of migrated mappings of old AILLA IDs to new fedora PIDs
 */

function getMediaInfo() {
	//this step occurs on DEV
	echo "Hi from inside getMediaInfo : Step 2 in restoring file restrictions \n";
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
	$query = 'SELECT * FROM utailla_media_file_map';
	$results = db_query($query);
	if ($results->rowCount() != 0) {
		foreach ($results as $record) {

			$refid = $record->media_file_id;
			$mediaPID = $record->pid;
			$mediaInfo[$mediaPID]['map'] = $refid ;
			$mediaInfo[$mediaPID]['pid'] = $mediaPID ; 
		}
	}
	
	var_dump($mediaInfo);

	return $mediaInfo;

}

/**
 * Step 3 in restoring file restrictions.
 * gets info on currently restricted media on PROD to avoid overwriting & duplication
 */

function getRestrictedOBJs() {
	//this step occurs on QA/PROD
	echo "Hi from inside getRestrictedOBJs : Step 3 in restoring file restrictions \n";
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
	$query = 'SELECT pid, level FROM utailla_media_file_restrictions';
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
 * Step 4 in restoring file restrictions.
 * writes to utailla_media_file_restrictions TABLE to set level 0 (no restriction) 
 */

function tempRestrictions($mediaOBJ) 
{

	//this step occurs on QA/PROD
	echo "Hi from inside tempRestrictions : Step 4 in restoring file restrictions \n";
	db_set_active();

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
 	$level = 0;
// 	$hint = $config['ingestHint'];
// 	$password = $config['ingestPass'] ;
// 	$pid = $object->id;
// 	$option = 8;
// 	$reason = 'Administrative curation in progress';
	
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
    
	$ct = 0;
	foreach ($mediaOBJ as $pid) {
		$ct++;
//		if ($ct > 25) { break; }
		
		$query="INSERT INTO {utailla_media_file_restrictions} (pid, level) VALUES ('$pid', '$level')" ;
		$result = db_query($query);

	}
	
}

/**
 * 
 * gets security info from OLDAILLA db FROM media_file 
 */

function getPassInfo($map) {
	//this step occurs on PROD 
//	echo "Hi from inside getPassInfo  \n";
//	db_set_active();

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
    // Database information
    $dbDriver  = $config['database']['driver'];
    $dbHost = $config['database']['host'];
    $dbUsr  = $config['database']['usr'];
    $dbPwd  = $config['database']['pwd'];
    $dbName  = "OLDAILLA";

    // connect to database
    $utailla_database = array(
        'database' => $dbName,
        'username' => $dbUsr,
        'password' => $dbPwd,
        'host' => $dbHost,
        'driver' => $dbDriver,
    );
	
    Database::addConnectionInfo('OLDUTAILLA', 'default', $utailla_database);
    db_set_active('OLDUTAILLA');

	$mediaInfo = array();
	$query = 'SELECT * FROM media_file WHERE media_file_id = :map';
	$results = db_query($query, array(':map' => $map));
	if ($results->rowCount() != 0) {
		foreach ($results as $record) {
			$refid = $record->media_file_id;
			$mediaInfo['map'] = $refid ;
			$mediaInfo['level']  = $record->level;
			$mediaInfo['time_limit'] = $record->time_limit;
			$mediaInfo['password'] = $record->password;
			$mediaInfo['password_hint'] = $record->password_hint;
			$mediaInfo['access_justification'] = $record->access_justification;
			$mediaInfo['justification_other'] = $record->justification_other;
		}
	}
	
//	echo ("mediaInfo is " . count($mediaInfo) . " when level is $level\n"); 
//	var_dump($mediaInfo);
	return $mediaInfo;

}

/**
 * 
 * re-adds level restrictions to media file restrictions according to migrated data on OLDAILLA 
 */

function updateRes($rI, $pid) 
{
	//this step occurs on QA/PROD
	db_set_active();

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
// 	$level = 1;
// 	$hint = $config['ingestHint'];
// 	$password = $config['ingestPass'] ;
// 	$pid = $object->id;
// 	$option = 8;
// 	$reason = 'Administrative curation in progress';

// array(7) {
//   ["map"]=>
//   string(6) "118178"
//   ["level"]=>
//   string(1) "2"
//   ["time_limit"]=>
//   string(0) ""
//   ["password"]=>
//   string(0) ""
//   ["password_hint"]=>
//   string(0) ""
//   ["access_justification"]=>
//   string(1) "0"
//   ["justification_other"]=>
//   string(0) ""
// }
	
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
	if ( ($rI['level'] === NULL) || ($rI['level'] == "0") ) {
		$level = 0 ;
	} else {
		$level = $rI['level'] - 1 ;
	}
	
// 	$time = $rI['time_limit'];
// 	$password = $rI['password'];
// 	$hint = $rI['password_hint'];
// 	$option = $rI['access_justification'];
// 	$reason = $rI['justification_other'];
//	$dName = $rI['depositor_name'];
//	$dEmail = $rI['depositor_email'];

//	echo ("level=$level \t password=$password \t time=$time \t hint=$hint \t option=$option \t reason=$reason \t pid=$pid \t depositor_name=dName \t depositor_email=$dEmail \n");		

 	$query = 'UPDATE utailla_media_file_restrictions SET level = :level WHERE pid = :pid';
 	$results = db_query($query, array(':level' => $level, ':pid' => $pid));
 	$results = db_update('utailla_media_file_restrictions') ;

	
}

/**
 * 
 * adds missing level restrictions to media file restrictions table according to migrated data on OLDAILLA 
 */

function newRes($rI, $pid) 
{
	//this step occurs on QA/PROD
	db_set_active();

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
 	$defaultHint = $config['ingestHint'];
 	$defaultPassword = $config['ingestPass'] ;

// array(7) {
//   ["map"]=>
//   string(6) "118178"
//   ["level"]=>
//   string(1) "2"
//   ["time_limit"]=>
//   string(0) ""
//   ["password"]=>
//   string(0) ""
//   ["password_hint"]=>
//   string(0) ""
//   ["access_justification"]=>
//   string(1) "0"
//   ["justification_other"]=>
//   string(0) ""
// }
	
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
	
	if (strstr($rI['time_limit'], '-')) {
		$date = $rI['time_limit'] ;
		$date = new DateTime($date);
		$time = $date->getTimestamp() ;
		$time = (int)$time;

	} else {
		$time = (int)$rI['time_limit'];
	}
	
	if ( ($rI['password'] === NULL) || ($rI['password'] == "") ) {
		$password = $defaultPassword;
		$hint = $defaultHint;
	} else {
		$password = $rI['password'];
		$hint = $rI['password_hint'];
	}

	if ( ($rI['level'] === NULL) || ($rI['level'] == "0") ) {
		$level = 0 ;
		$password = "";
		$hint = "";
	} else {
		$level = $rI['level'] - 1 ;
	}
	
	$option = $rI['access_justification'];
	$reason = $rI['justification_other'];

	echo ("level=$level \t password=$password \t time=$time \t hint=$hint \t option=$option \t reason=$reason \t pid=$pid \n");		

$query="INSERT INTO {utailla_media_file_restrictions} (pid, level, end_time, password, 	hint, reason_option, custom_reason) VALUES ('$pid', '$level', '$time', '$password', '$hint', '$option', '$reason')" ;
$result = db_query($query);

	
}

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
 * determines if the PID is in the current media files restrictions table
 */

function findPIDinRes($pid) {
	//this step occurs on QA/PROD
	//echo "Hi from inside findPIDinRes  \n";
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

	$mediaInfo = FALSE;
	$query = 'SELECT * FROM utailla_media_file_restrictions WHERE pid = :pid';
	$results = db_query($query, array(':pid' => $pid));

	if ($results->rowCount() != 0) {
		$mediaInfo = TRUE;
	}

	return $mediaInfo;

}

/**
 * finds the resource for the media missing restriction Info
 */
function findResource4Missing($m) {

	db_set_active();

	module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');
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
    
    
	$mediaOBJ = array();
	$resource = FALSE;
	
	$object = islandora_object_load($m);
	if (!$object) {
	  echo ("PID -- " . $m . " did not load\n");
	  continue;
	}
	//echo ("PID -- " . $object->id . "\n");
	
	$mediaOBJ = $object->getParents();
	$tmp = $mediaOBJ[0];
	$resource = $object->id ;

	return $resource;

}

function resourceMap($resKeys) {
	//this step occurs on QA/PROD
	echo "Hi from inside resourceMap : Step 7.2 in restoring file restrictions \n";
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
	$query = 'SELECT * FROM utailla_resource_map';
	$results = db_query($query);
	if ($results->rowCount() != 0) {
		foreach ($results as $record) {
			$refid = $record->resource_id;
			$mediaPID = $record->pid;

			if (in_array($mediaPID, $resKeys)) {
				$mediaInfo[$mediaPID]['map'] = $refid ;
			}
		}
	}
	
	return $mediaInfo;


}

function getResourceInfo($map) {
	//this step occurs on QA 
//	echo "Hi from inside getResourceInfo : Step 7.4 in restoring file restrictions \n";
//	db_set_active();

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
    // Database information
    $dbDriver  = $config['database']['driver'];
    $dbHost = $config['database']['host'];
    $dbUsr  = $config['database']['usr'];
    $dbPwd  = $config['database']['pwd'];
    $dbName  = "OLDAILLA";

    // connect to database
    $utailla_database = array(
        'database' => $dbName,
        'username' => $dbUsr,
        'password' => $dbPwd,
        'host' => $dbHost,
        'driver' => $dbDriver,
    );
	
    Database::addConnectionInfo('OLDUTAILLA', 'default', $utailla_database);
    db_set_active('OLDUTAILLA');
	$collectionID = array();
//	$query = 'SELECT DISTINCT contributor_id FROM contributor_to_resource WHERE resource_id = :map AND role_id=2';
	$query = 'SELECT DISTINCT collection_id FROM collection_to_resource WHERE resource_id = :map';
	$results = db_query($query, array(':map' => $map));
	if ($results->rowCount() != 0) {
		foreach ($results as $record) {
			$collectionID[]  = $record->collection_id;
		}
	}
	
	return $collectionID;

}

function getResourceDepInfo($map) {
	//this step occurs on QA 
//	echo "Hi from inside getResourceInfo : Step 7.4 in restoring file restrictions \n";
//	db_set_active();

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
    // Database information
    $dbDriver  = $config['database']['driver'];
    $dbHost = $config['database']['host'];
    $dbUsr  = $config['database']['usr'];
    $dbPwd  = $config['database']['pwd'];
    $dbName  = "OLDAILLA";

    // connect to database
    $utailla_database = array(
        'database' => $dbName,
        'username' => $dbUsr,
        'password' => $dbPwd,
        'host' => $dbHost,
        'driver' => $dbDriver,
    );
	
    Database::addConnectionInfo('OLDUTAILLA', 'default', $utailla_database);
    db_set_active('OLDUTAILLA');
	$depositorID = array();
	$query = 'SELECT depositor_id FROM collection_to_depositor WHERE collection_id = :map AND role_id=2';
	$results = db_query($query, array(':map' => $map));
	if ($results->rowCount() != 0) {
		foreach ($results as $record) {
			$depositorID[]  = $record->depositor_id;
		}
	}
	
	return $depositorID;

}

function addDepositor($rap) {
	//this step occurs on QA/PROD
	echo "Hi from inside addDepositor : Step 7.4 in restoring file restrictions \n";
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
	
	foreach ($rap as $key => $r4m) {	
		echo("key is $key \n");
		var_dump($r4m);
		$tmpPass = user_hash_password($key);
		echo("$key has $tmpPass \n");
		$hint = "islandora" ;
		foreach ($r4m as $m) {	
			
 			$query = 'UPDATE utailla_media_file_restrictions SET hint = :hint, password = :password WHERE pid = :pid';
 			$results = db_query($query, array(':hint' => $hint, ':password' => $tmpPass, ':pid' => $m));
 			$results = db_update('utailla_media_file_restrictions') ;

		
		}
	}
	
	//return $mediaInfo;

}

function getLocInfo($map) {
	//this step occurs on DEV (not using this one currently)
//	echo "Hi from inside getLocInfo  \n";
//	db_set_active();

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
    // Database information
    $dbDriver  = $config['database']['driver'];
    $dbHost = $config['database']['host'];
    $dbUsr  = $config['database']['usr'];
    $dbPwd  = $config['database']['pwd'];
    $dbName  = "OLDAILLA";

    // connect to database
    $utailla_database = array(
        'database' => $dbName,
        'username' => $dbUsr,
        'password' => $dbPwd,
        'host' => $dbHost,
        'driver' => $dbDriver,
    );
	
    Database::addConnectionInfo('OLDUTAILLA', 'default', $utailla_database);
    db_set_active('OLDUTAILLA');

	$mediaInfo = array();
	$query = 'SELECT * FROM media_file WHERE media_file_id = :map';
	$results = db_query($query, array(':map' => $map));
	if ($results->rowCount() != 0) {
		foreach ($results as $record) {
			$mediaInfo['directory_path']  = $record->directory_path;
			$mediaInfo['identifier']  = $record->identifier;
			$mediaInfo['map']  = $record->media_file_id;
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
 * function to reingest proxy_mp3.
 *
*/
function proxyIngest($workFields)
{
	module_load_include('inc', 'utailla', 'includes/utilities');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');
	module_load_include('inc', 'islandora', 'includes/derivatives');
	module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    $namespace = $config['default_namespace'];

    $params = array();
    setup($config, $params);
    $fedoraConnection = $params['fedoraConnection'];
 
    $username = "fedoraAdmin";
    $now = date("m/d/Y h:i a");

    $pid = $workFields['pid'];
    $file = $workFields['file'];
	$object = $fedoraConnection->repository->getObject($pid);

    $datastreams = $object->repository->api->a->listDatastreams($pid);
	$mime = findMime($file) ;
    // Create/Update PROXY_MP3 datastream
    if (!isset($object["PROXY_MP3"])) {
        $datastream_id = "PROXY_MP3";
        $controlGroup = "M";
        $datastream = $object->constructDatastream($datastream_id, $controlGroup);
        $datastream->label = 'PROXY_MP3';
        $datastream->mimetype = $mime;
        $datastream->setContentFromFile($file);
        $object->ingestDatastream($datastream);
    } else {
        $datastream = $object["PROXY_MP3"];
        $datastream->label = 'PROXY_MP3';
        $datastream->mimetype = mime;
        $datastream->content = $file;
    }

}

/**
 * function to ingest restricted thumbnail.
 *
*/
function addResTN($pid)
{
	module_load_include('inc', 'utailla', 'includes/utilities');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');
	module_load_include('inc', 'islandora', 'includes/derivatives');
	module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    $namespace = $config['default_namespace'];

    $params = array();
    setup($config, $params);
    $fedoraConnection = $params['fedoraConnection'];
 	
    $username = "fedoraAdmin";
    $now = date("m/d/Y h:i a");

	$msg="";
  	$tnFile = $path . "images/noimg.png" ;
  	//print ("$tnFile is the file to ingest\n") ;
	
	$excludeList = array("ailla:154064", "ailla:234517", "ailla:237195", "ailla:237196", "ailla:237905", "ailla:237906", "ailla:242345", "ailla:242684", "ailla:242686", "ailla:242688", "ailla:242690", "ailla:242692", "ailla:242694", "ailla:242696", "ailla:242698", "ailla:242700", "ailla:242702", "ailla:242704", "ailla:242706", "ailla:242708", "ailla:242710", "ailla:242729", "ailla:242730", "ailla:242731", "ailla:242732", "ailla:242733", "ailla:242734", "ailla:242735", "ailla:242736", "ailla:242737", "ailla:242745", "ailla:242746", "ailla:242766", "ailla:242985", "ailla:242986", "ailla:243643", "ailla:243665", "ailla:243666", "ailla:243680", "ailla:243682", "ailla:243689", "ailla:243723", "ailla:244113", "ailla:244142", "ailla:244145", "ailla:244162", "ailla:244166", "ailla:244171", "ailla:244188", "ailla:244191", "ailla:244194", "ailla:244202", "ailla:244221", "ailla:244251", "ailla:244252", "ailla:244253", "ailla:244279", "ailla:244299", "ailla:244323", "ailla:244329", "ailla:244332", "ailla:244339", "ailla:244340", "ailla:244356", "ailla:244370", "ailla:244400", "ailla:244411", "ailla:244443", "ailla:244444", "ailla:244459", "ailla:244460", "ailla:244482", "ailla:244496", "ailla:244533", "ailla:244557", "ailla:244559", "ailla:244587", "ailla:244588", "ailla:244606", "ailla:244615", "ailla:244639", "ailla:244641", "ailla:244648", "ailla:244651", "ailla:244674", "ailla:244690", "ailla:244714", "ailla:244715", "ailla:244716", "ailla:244717", "ailla:244719", "ailla:244793", "ailla:244794", "ailla:244795", "ailla:244796", "ailla:244797", "ailla:244798", "ailla:244799", "ailla:244800", "ailla:244801", "ailla:244829", "ailla:244832", "ailla:244850", "ailla:244853", "ailla:244854", "ailla:244855", "ailla:244856", "ailla:244857", "ailla:244858", "ailla:244859", "ailla:244860", "ailla:244861", "ailla:244862", "ailla:244865", "ailla:244867", "ailla:244997", "ailla:244998", "ailla:244999", "ailla:245000", "ailla:245001", "ailla:245003", "ailla:245005", "ailla:245012", "ailla:176109", "ailla:242569", "ailla:242570", "ailla:242634", "ailla:242686", "ailla:242688", "ailla:242690", "ailla:242692", "ailla:242694", "ailla:242696", "ailla:242698", "ailla:242700", "ailla:242702", "ailla:242704", "ailla:242706", "ailla:242708", "ailla:242710", "ailla:243270", "ailla:243518", "ailla:243643", "ailla:243759", "ailla:243906", "ailla:243981");

	if (in_array($pid, $excludeList)) {
//		$msg = $pid . " is not in Fedora\n" ;
    	return $msg;  
	}

	$object = $fedoraConnection->repository->getObject($pid);
	
    if (!isset($object["OBJ"])) { 
    	$msg = "Now working with Archival PID " . $pid . "\n" ;
//		$arcCT++;
		//Create/Update TN datastream
		if (!isset($object["TN"])) {
			$datastream_id = "TN";
			$controlGroup = "M";
			$datastream = $object->constructDatastream($datastream_id, $controlGroup);
			$datastream->label = 'Thumbnail';
			$datastream->mimetype = 'image/png';
			$datastream->setContentFromFile($tnFile);
			$object->ingestDatastream($datastream);
		} else {
			$datastream = $object["TN"];
			$datastream->label = 'Thumbnail';
			$datastream->mimetype = 'image/png';
			$datastream->setContentFromFile($tnFile);
		}	
    	return $msg;  
    }
    
    return ("not archival $pid\n");
    //$datastreams = $object->repository->api->a->listDatastreams($pid);
	//var_dump($datastreams);

//	$msg = "Now working with PID " . $pid . "\n" ;
    //$labelTN = 
//     
//     //Create/Update TN datastream
//     if (!isset($object["TN"])) {
//         $datastream_id = "TN";
//         $controlGroup = "M";
//         $datastream = $object->constructDatastream($datastream_id, $controlGroup);
//         $datastream->label = 'Restricted Thumbnail';
//         $datastream->mimetype = 'image/png';
//         $datastream->setContentFromFile($tnFile);
//         $object->ingestDatastream($datastream);
//     } else {
//         $datastream = $object["TN"];
//         $datastream->label = 'Restricted Thumbnail';
//         $datastream->mimetype = 'image/png';
//         $datastream->setContentFromFile($tnFile);
//     }	
    
//    return $msg;
	
}

/**
 * function to ingest rels-int to unrestricted objs.
 *
*/
function addRels($pid)
{
	module_load_include('inc', 'utailla', 'includes/utilities');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');
	module_load_include('inc', 'islandora', 'includes/derivatives');
	module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    $namespace = $config['default_namespace'];

    $params = array();
    setup($config, $params);
    $fedoraConnection = $params['fedoraConnection'];
 	
    $username = "fedoraAdmin";
    $now = date("m/d/Y h:i a");

	$msg="";
	
	$excludeList = array("ailla:154064", "ailla:234517", "ailla:237195", "ailla:237196", "ailla:237905", "ailla:237906", "ailla:242345", "ailla:242684", "ailla:242686", "ailla:242688", "ailla:242690", "ailla:242692", "ailla:242694", "ailla:242696", "ailla:242698", "ailla:242700", "ailla:242702", "ailla:242704", "ailla:242706", "ailla:242708", "ailla:242710", "ailla:242729", "ailla:242730", "ailla:242731", "ailla:242732", "ailla:242733", "ailla:242734", "ailla:242735", "ailla:242736", "ailla:242737", "ailla:242745", "ailla:242746", "ailla:242766", "ailla:242985", "ailla:242986", "ailla:243643", "ailla:243665", "ailla:243666", "ailla:243680", "ailla:243682", "ailla:243689", "ailla:243723", "ailla:244113", "ailla:244142", "ailla:244145", "ailla:244162", "ailla:244166", "ailla:244171", "ailla:244188", "ailla:244191", "ailla:244194", "ailla:244202", "ailla:244221", "ailla:244251", "ailla:244252", "ailla:244253", "ailla:244279", "ailla:244299", "ailla:244323", "ailla:244329", "ailla:244332", "ailla:244339", "ailla:244340", "ailla:244356", "ailla:244370", "ailla:244400", "ailla:244411", "ailla:244443", "ailla:244444", "ailla:244459", "ailla:244460", "ailla:244482", "ailla:244496", "ailla:244533", "ailla:244557", "ailla:244559", "ailla:244587", "ailla:244588", "ailla:244606", "ailla:244615", "ailla:244639", "ailla:244641", "ailla:244648", "ailla:244651", "ailla:244674", "ailla:244690", "ailla:244714", "ailla:244715", "ailla:244716", "ailla:244717", "ailla:244719", "ailla:244793", "ailla:244794", "ailla:244795", "ailla:244796", "ailla:244797", "ailla:244798", "ailla:244799", "ailla:244800", "ailla:244801", "ailla:244829", "ailla:244832", "ailla:244850", "ailla:244853", "ailla:244854", "ailla:244855", "ailla:244856", "ailla:244857", "ailla:244858", "ailla:244859", "ailla:244860", "ailla:244861", "ailla:244862", "ailla:244865", "ailla:244867", "ailla:244997", "ailla:244998", "ailla:244999", "ailla:245000", "ailla:245001", "ailla:245003", "ailla:245005", "ailla:245012", "ailla:176109", "ailla:242569", "ailla:242570", "ailla:242634", "ailla:242686", "ailla:242688", "ailla:242690", "ailla:242692", "ailla:242694", "ailla:242696", "ailla:242698", "ailla:242700", "ailla:242702", "ailla:242704", "ailla:242706", "ailla:242708", "ailla:242710", "ailla:243270", "ailla:243518", "ailla:243643", "ailla:243759", "ailla:243906", "ailla:243981");

	if (in_array($pid, $excludeList)) {
		$msg = $pid . " is not in Fedora\n" ;
    	return $msg;  
	}

	$content = '<rdf:RDF xmlns:islandora="http://islandora.ca/ontology/relsint#" xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">' ;
	$content .= "\n" ;
	$content .= '<rdf:Description rdf:about="info:fedora/' . $pid . '/OBJ">' ;
	$content .= "\n" ;
	$content .= "<islandora:isViewableByUser>fedoraAdmin</islandora:isViewableByUser>\n" ;
	$content .= "<islandora:isViewableByRole>authenticated user</islandora:isViewableByRole>\n" ;
	$content .= "<islandora:isViewableByRole>administrator</islandora:isViewableByRole>\n" ;
	$content .= "</rdf:Description>\n" ;
	$content .= "</rdf:RDF>\n" ;

//	var_dump($content);
	
	$object = $fedoraConnection->repository->getObject($pid);
	if (!($object)) {
//		$msg = $pid . " is not in Fedora\n" ;
    	return $msg;  
	}

    if (!isset($object["OBJ"])) { 
//		$msg = $pid . " is not a media file\n" ;
    	return $msg;  
    }
    
    //$datastreams = $object->repository->api->a->listDatastreams($pid);
	//var_dump($datastreams);

	$msg = "\nNow working with PID " . $pid . "\n" ;
	
	$dsRELS = $object['RELS-INT']->content ;
	
	if (strpos($dsRELS, 'isViewableByRole') === false) {
		echo $msg;
		var_dump($dsRELS) ;
		echo "False\n";
		//Create/Update TN datastream
		if (!isset($object["RELS-INT"])) {
			$datastream_id = "RELS-INT";
			$controlGroup = "M";
			$datastream = $object->constructDatastream($datastream_id, $controlGroup);
			$datastream->label = 'Fedora Relationship Metadata.';
			$datastream->mimetype = 'application/rdf+xml';
			$datastream->setContentFromString($content);
			$object->ingestDatastream($datastream);
		} else {
			$datastream = $object["RELS-INT"];
			$datastream->label = 'Fedora Relationship Metadata.';
			$datastream->mimetype = 'application/rdf+xml';
			$datastream->setContentFromString($content);
		}	
		echo($content);
	}    
    return $msg;
	
}

/**
 * function to ingest xacml POLICY datastream to unrestricted objs.
 *
*/
function addPOLs($pid, $content)
{
	module_load_include('inc', 'utailla', 'includes/utilities');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');
	module_load_include('inc', 'islandora', 'includes/derivatives');
	module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    $namespace = $config['default_namespace'];

    $params = array();
    setup($config, $params);
    $fedoraConnection = $params['fedoraConnection'];
 	
    $username = "fedoraAdmin";
    $now = date("m/d/Y h:i a");

	$msg="";
	
	$excludeList = array("ailla:154064", "ailla:234517", "ailla:237195", "ailla:237196", "ailla:237905", "ailla:237906", "ailla:242345", "ailla:242684", "ailla:242686", "ailla:242688", "ailla:242690", "ailla:242692", "ailla:242694", "ailla:242696", "ailla:242698", "ailla:242700", "ailla:242702", "ailla:242704", "ailla:242706", "ailla:242708", "ailla:242710", "ailla:242729", "ailla:242730", "ailla:242731", "ailla:242732", "ailla:242733", "ailla:242734", "ailla:242735", "ailla:242736", "ailla:242737", "ailla:242745", "ailla:242746", "ailla:242766", "ailla:242985", "ailla:242986", "ailla:243643", "ailla:243665", "ailla:243666", "ailla:243680", "ailla:243682", "ailla:243689", "ailla:243723", "ailla:244113", "ailla:244142", "ailla:244145", "ailla:244162", "ailla:244166", "ailla:244171", "ailla:244188", "ailla:244191", "ailla:244194", "ailla:244202", "ailla:244221", "ailla:244251", "ailla:244252", "ailla:244253", "ailla:244279", "ailla:244299", "ailla:244323", "ailla:244329", "ailla:244332", "ailla:244339", "ailla:244340", "ailla:244356", "ailla:244370", "ailla:244400", "ailla:244411", "ailla:244443", "ailla:244444", "ailla:244459", "ailla:244460", "ailla:244482", "ailla:244496", "ailla:244533", "ailla:244557", "ailla:244559", "ailla:244587", "ailla:244588", "ailla:244606", "ailla:244615", "ailla:244639", "ailla:244641", "ailla:244648", "ailla:244651", "ailla:244674", "ailla:244690", "ailla:244714", "ailla:244715", "ailla:244716", "ailla:244717", "ailla:244719", "ailla:244793", "ailla:244794", "ailla:244795", "ailla:244796", "ailla:244797", "ailla:244798", "ailla:244799", "ailla:244800", "ailla:244801", "ailla:244829", "ailla:244832", "ailla:244850", "ailla:244853", "ailla:244854", "ailla:244855", "ailla:244856", "ailla:244857", "ailla:244858", "ailla:244859", "ailla:244860", "ailla:244861", "ailla:244862", "ailla:244865", "ailla:244867", "ailla:244997", "ailla:244998", "ailla:244999", "ailla:245000", "ailla:245001", "ailla:245003", "ailla:245005", "ailla:245012", "ailla:176109", "ailla:242569", "ailla:242570", "ailla:242634", "ailla:242686", "ailla:242688", "ailla:242690", "ailla:242692", "ailla:242694", "ailla:242696", "ailla:242698", "ailla:242700", "ailla:242702", "ailla:242704", "ailla:242706", "ailla:242708", "ailla:242710", "ailla:243270", "ailla:243518", "ailla:243643", "ailla:243759", "ailla:243906", "ailla:243981");

	if (in_array($pid, $excludeList)) {
		$msg = $pid . " is not in Fedora\n" ;
    	return $msg;  
	}

//	var_dump($content);
	
	$object = $fedoraConnection->repository->getObject($pid);
	if (!($object)) {
		$msg = $pid . " is not in Fedora\n" ;
    	return $msg;  
	}

    if (!isset($object["OBJ"])) { 
		$msg = $pid . " is not a media file\n" ;
    	return $msg;  
    }
    
	$msg = "\nNow working with PID " . $pid . "\n" ;
	echo $msg;

	//Create/Update TN datastream
	if (!isset($object["POLICY"])) {
		$datastream_id = "POLICY";
		$controlGroup = "M";
		$datastream = $object->constructDatastream($datastream_id, $controlGroup);
		$datastream->label = 'XACML Policy Stream';
		$datastream->mimetype = 'application/xml';
		$datastream->setContentFromString($content);
		$object->ingestDatastream($datastream);
	} else {
		$datastream = $object["POLICY"];
		$datastream->label = 'XACML Policy Stream';
		$datastream->mimetype = 'application/xml';
		$datastream->setContentFromString($content);
	}	
    return $msg;
	
}

/**
 * function to reingest thumbnail.
 *
*/
function findOBJnoTN()
{
	db_set_active();

	module_load_include('inc', 'utailla', 'includes/batch_resource_ingest');
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'islandora', 'includes/tuque');
    module_load_include('php', 'islandora_solr', 'SolrPhpClient/Apache/Solr/Service');

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    require_once $path . 'includes/fedora_connection.inc';

    $config = parse_ini_file($path . 'includes/config.ini', true);

    //find images with the NEEDOBJ datastream
    $imageNOs = array();
    $queryNO = 'RELS_EXT_hasModel_uri_ss:"info:fedora/islandora:sp_basic_image"' ;
    $solrLimit = 50000;
    $params = array();
    $solrNO = new Apache_Solr_Service('localhost', 8080, '/solr');
    try {
        $resultsNO = $solrNO->search($queryNO, 0, $solrLimit, $params,  'GET');
    } catch (Exception $ex) {
        drupal_set_message($ex->__toString() . PHP_EOL);
    }

    foreach ($resultsNO->response->docs as $doc) {
		$test = $doc->__get('PID') ;
        $imageNOs[] = $doc->__get('PID');
    }

    print ("started with " . count($imageNOs) . " imageNOs\n");
    sort($imageNOs);
//    var_dump($imageNOs);
    return $imageNOs;

	
}
