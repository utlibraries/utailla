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
