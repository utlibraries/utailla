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
        drupal_set_message("Fedora Object isn't in the repo!");
        return;
    }

	$relsDatastream = $object['RELS-EXT'];
	$relsResult = $relsDatastream->content;
	$relsInfo = parse_relsExt_for_Cmodel($relsResult);
	
	$objPath = 'http://' . $_SERVER['HTTP_HOST'] . "/islandora/object/";

	if ($relsInfo == 'info:fedora/ailla:resourceCModel') {
		$query = 'select $pid from <#ri> where $pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/'.$objectID.'>';
		$result = $repository->ri->itqlQuery($query);
		
		if (!empty($result)) {
			$caption = "There are " . count($result) . " Objects in this Resource<br />";
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
		} else {
			$block_title = '<h2>Media Files</h2>';
		}

		$header = array("Object", "File Types", "Access Level") ;
		$output = theme('table', array(
						'header' => $header,
						'rows'   => $rows,
						'attributes' => array('width' => '100%')
					));
		$output = $block_title . $caption . $output;
	
	} else {
		$output = "" ;
	}

	return $output;
}
