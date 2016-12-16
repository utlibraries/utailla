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
    module_load_include('php', 'islandora_solr', 'SolrPhpClient/Apache/Solr/Service');

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
	$query = 'SELECT pid FROM utailla_media_file_restrictions';
	$results = db_query($query);

	if ($results->rowCount() != 0) {
		foreach ($results as $record) {
			$pid = $record->pid;
			$mediaInfo[] = $pid ;
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
		if ($ct > 25) { break; }
		
		$query="INSERT INTO {utailla_media_file_restrictions} (pid, level) VALUES ('$pid', '$level')" ;
		$result = db_query($query);

	}
	
}

/**
 * Step 5 in restoring file restrictions.
 * gets security info from OLDAILLA db FROM media_file WHERE archival_status = N
 */

function getPassInfo($level) {
	//this step occurs on DEV (not using this one currently)
	echo "Hi from inside getPassInfo : Step 5 in restoring file restrictions \n";
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

// media_file_id, archival_status, level, time_limit, password, password_hint, 
// access_justification, justification_other

	$mediaInfo = array($level);
	$query = 'SELECT * FROM media_file WHERE archival_object = :archive AND level = :level';
	$results = db_query($query, array(':archive' => "N", ':level' => $level));
	if ($results->rowCount() != 0) {
		foreach ($results as $record) {
			$refid = $record->media_file_id;
			$mediaInfo[$refid]['level']  = $record->level;
			$mediaInfo[$refid]['time_limit'] = $record->time_limit;
			$mediaInfo[$refid]['password'] = $record->password;
			$mediaInfo[$refid]['password_hint'] = $record->password_hint;
			$mediaInfo[$refid]['access_justification'] = $record->access_justification;
			$mediaInfo[$refid]['justification_other'] = $record->justification_other;
			$mediaInfo[$refid]['map'] = $refid ;
		}
	}
	
	echo ("mediaInfo is " . count($mediaInfo) . " when level is $level\n"); 
//	var_dump($mediaInfo);
	return $mediaInfo;

}

/**
 * Step 6 in restoring file restrictions.
 * Updates migrated media restrictions (levels 2-4) on utailla_media_file_restrictions
 * with correct levels & passwords where available 
 */

function reAddRestrictions($restrictionInfo) 
{
	//this step occurs on QA/PROD
	echo "Hi from inside getRestrictedOBJs : Step 6 in restoring file restrictions \n";
	db_set_active();

    $path = DRUPAL_ROOT . DIRECTORY_SEPARATOR . drupal_get_path('module', 'utailla') . DIRECTORY_SEPARATOR;
    $config = parse_ini_file($path . 'includes/config.ini', true);
    
// 	$level = 1;
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

	$query = 'UPDATE utailla_media_file_restrictions SET level = :level, hint = :hint, password = :password, reason_option = :option, custom_reason = :reason WHERE pid = :pid';
	$results = db_query($query, array(':level' => $level, ':hint' => $hint, ':password' => $password, ':pid' => $pid, ':option' => $option, ':reason' => $reason));
	$results = db_update('utailla_media_file_restrictions') ;

	
}


/**
 * Updates all out of date restrictions.
 */
function utailla_remove_outdated_embargo_restrictions() {
  db_update('utailla_media_file_restrictions')
    ->fields(
      array(
        'level' => UTAILLA_RESTRICTION_LEVEL_1,
        'end_time' => NULL,
        'password' => NULL,
        'hint' => NULL,
      )
    )
    ->condition('level', UTAILLA_RESTRICTION_LEVEL_3)
    ->condition('end_time', time(), '<')
    ->execute();
}

