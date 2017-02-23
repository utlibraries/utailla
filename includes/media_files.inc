<?php

/**
* @file
* Creates Resource Media Files Listing
**/
function utailla_media_files()
{
    module_load_include('inc', 'islandora', 'includes/IslandoraTuque');
    module_load_include('inc', 'utailla', 'includes/metadata_functions');
    $my_islandora_tuque = new IslandoraTuque();
    $repository = $my_islandora_tuque->repository;
	$output = "";
	$rows = array();
	
	$current_url = $_SERVER['REQUEST_URI'];
	$path = explode('/', $current_url);
	$lang_path = $path[1];
	if ($lang_path == 'es') {
		$langSwitch = 'es';
	} else if ($lang_path == 'islandora') {
		$langSwitch = 'eng';
	}

	$object = menu_get_object('islandora_object', 2);
	$objectID = $object->id;
    if (!$object) {
        //drupal_set_message("Fedora Object isn't in the repo!");
        return;
    }

	$relsDatastream = $object['RELS-EXT'];
	$relsResult = $relsDatastream->content;
	$relsInfo = parse_relsExt_for_Cmodel($relsResult);
	
	if ($langSwitch == 'es') {
		$objPath = 'http://' . $_SERVER['HTTP_HOST'] . "/es/islandora/object/";
	} else {
		$objPath = 'http://' . $_SERVER['HTTP_HOST'] . "/islandora/object/";
	}

	if ($relsInfo == 'info:fedora/ailla:resourceCModel') {
		$query = 'select $pid from <#ri> where $pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/'.$objectID.'>';
		$result = $repository->ri->itqlQuery($query);
		
		if (!empty($result)) {
			if ($langSwitch == 'es') {
				$caption = "Hay " . count($result) . " objetos en este recurso<br />";
			} else {
				$caption = "There are " . count($result) . " objects in this resource<br />";
			}
 			foreach ($result as $r) {

 				$mediaPID = $r['pid']['value'] ;
 				$mediaURL = $objPath . $mediaPID ;
				$mediaOBJ = $repository->getObject($mediaPID);
				$datastream = $mediaOBJ['OBJ'] ;
				$accessLevel = getMediaAccess($mediaPID) + 1;
				$label = $mediaOBJ->label ;
				$mimetype = $datastream->mimetype ;
				$link = '<a href="' . $mediaURL . '">' . $label . "</a>" ;			
				$rows[] = array($link, $mimetype, $accessLevel)  ;
 			}
		} else {
			$emptyResult = TRUE;
		}
	
		if ($langSwitch == 'es') {
			$block_title = '<h2>Listado de Medios</h2>';
			$header = array("Objeto", "Tipos de Archivo", "Nivel de Acceso") ;
		} else {
			$block_title = '<h2>Media Files</h2>';
			$header = array("Object", "File Types", "Access Level") ;
		}


		$per_page = 100;
		// Initialize the pager
		$current_page = pager_default_initialize(count($rows), $per_page);
		// Split your list into page sized chunks
		$chunks = array_chunk($rows, $per_page, TRUE);
		// Show the appropriate items from the list
		$output = theme('table', array('header' => $header, 'rows' => $chunks[$current_page]));
		// Show the pager
		$output .= theme('pager', array('quantity',count($rows)));
		
		$output = $block_title . $caption . $output;
	
	} else {
		$output = "" ;
	}

	return $output;
}
