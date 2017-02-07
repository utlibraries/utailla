<?php

/**
 * @file
 * Allows for a downloadable link for OBJs.
 */


function utailla_object_download()
{
    module_load_include('inc', 'utailla', 'includes/metadata_functions');
	$currentPage = $_SERVER['REQUEST_URI'];
	$path = explode('/', $currentPage);
	$info = array();
	$output = "" ;
	
	$lang_path = $path[1];
	if ($lang_path == 'es') {
		$info['lang'] = 'es';
	} else if ($lang_path == 'islandora') {
		$info['lang'] = 'eng';
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
	$currentURL = 'http://' .$_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI'];
	
	switch ($relsInfo) {
		case 'info:fedora/islandora_sp-audioCModel':
			$info['objType'] = "Audio";
			$info['streamURL'] = $currentURL . '/datastream/PROXY_MP3/download'    ;
			$info['objURL'] = $currentURL . '/datastream/OBJ/download'   ;
			
			break;
		case 'info:fedora/islandora_sp_pdf':
			$info['objType'] = "PDF";
			$info['objURL'] = $currentURL . '/datastream/OBJ/download'   ;
			
			break;
		case 'info:fedora/islandora_sp_videoCModel':
			$info['objType'] = "Video";
			$info['streamURL'] = $currentURL . '/datastream/MP4/download'    ;
			$info['objURL'] = $currentURL . '/datastream/OBJ/download'   ;
			
			break;
		case 'info:fedora/islandora_binaryObjectCModel':
			$info['objType'] = "Binary";
			$info['objURL'] = $currentURL . '/datastream/OBJ/download'   ;
			
			break;
		case 'info:fedora/islandora_sp_document':
			$info['objType'] = "Document";
			$info['objURL'] = $currentURL . '/datastream/OBJ/download'   ;
			
			break;
		case 'info:fedora/islandora_sp_basic_image':
			$info['objType'] = "Image";
			$info['streamURL'] = $currentURL . '/datastream/MEDIUM_SIZE/download'    ;
			$info['objURL'] = $currentURL . '/datastream/OBJ/download'   ;
			
			break;
		case 'info:fedora/islandora_sp_large_image_cmodel':
			$info['objType'] = "Image";
			$info['streamURL'] = $currentURL . '/datastream/JPG/download'    ;
			$info['objURL'] = $currentURL . '/datastream/OBJ/download'   ;
			
			break;
		default:
			$info['objType'] = "other";
			
			break;
	}
	
	foreach ($info as $key =>$value) {
		switch ($key) {
			case 'streamURL':
				$rows[] = '<a href="' . $value . '">Download ' . $info['objType'] . "</a><br />";
				break;
			case 'objURL':
				$rows[] = '<a href="' . $value . '">Download Large Object</a><br />';
			
				break;
			default:
				break;
		}
	}
	
	
	$header = '';
	$output = theme('table', array(
					'header' => $header,
					'rows'   => $rows,
					'attributes' => array('width' => '100%')
				));
	
	return $output;
}

