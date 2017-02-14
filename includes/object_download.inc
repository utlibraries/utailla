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
//        drupal_set_message("Fedora Object isn't in the repo!");
        return;
    }
	$label = $object->label;
	
	$relsDatastream = $object['RELS-EXT'];
	$relsResult = $relsDatastream->content;
	$relsInfo = parse_relsExt_for_Cmodel($relsResult);
	$currentURL = 'http://' . $_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI'];
	
	switch ($relsInfo) {
		case 'info:fedora/islandora_sp-audioCModel':
			$info['objType'] = "Audio";
			$info['streamURL'] = $currentURL . '/datastream/PROXY_MP3/download/' . $label    ;
			$info['objURL'] = $currentURL . '/datastream/OBJ/download/' . $label   ;
			
			if ($info['lang'] == "es") {
				$link = '<a href="' . $info['objURL'] . '">Descarga ' . $label . " Versión del Archivo</a><br />\n" ;	
//				$link .= '<a href="' . $info['streamURL'] . '" download="' . $label . '">Descarga ' . "PROXY_MP3 Versión MP3</a>" ;					
			} else {
				$link = '<a href="' . $info['objURL'] . '">Download ' . $label . " Archival Version</a><br />\n" ;
//				$link .= '<a href="' . $info['streamURL'] . '" download="' . $label . '">Download ' . "PROXY_MP3 MP3 Version</a>" ;
			}
			$output .= $link . "<br />";
			break;
		case 'info:fedora/islandora_sp_pdf':
			$info['objType'] = "PDF";
			$info['streamURL'] = $currentURL . '/datastream/OBJ/download/' . $label   ;
			
			if ($info['lang'] == "es") {
				$link = '<a href="' . $info['streamURL'] . '">Descarga ' . $label . " PDF</a><br />" ;					
			} else {
//				$link = '<a href="' . $info['streamURL'] . '" download="' . $label . '">Download ' . $label . " PDF</a>" ;
				$link = "" ;
			}
			$output .= $link . "<br />";
			break;
		case 'info:fedora/islandora_sp_videoCModel':
			$info['objType'] = "Video";
			$info['streamURL'] = $currentURL . '/datastream/MP4/download/' . $label    ;
			$info['objURL'] = $currentURL . '/datastream/OBJ/download/' . $label   ;
			
			if ($info['lang'] == "es") {
				$link = '<a href="' . $info['objURL'] . '">Descarga ' . $label . " Versión del Archivo</a><br />\n" ;	
//				$link .= '<a href="' . $info['streamURL'] . '" download="' . $label . '">Descarga ' . " Versión MP4</a>" ;					
			} else {
				$link = '<a href="' . $info['objURL'] . '">Download ' . $label . " Archival Version</a><br />\n" ;
//				$link .= '<a href="' . $info['streamURL'] . '" download="' . $label . '">Download ' . " MP4 Version</a>" ;
			}
			$output .= $link . "<br />";
			break;
		case 'info:fedora/islandora_sp_document':
			$info['objType'] = "Document";
			$info['streamURL'] = $currentURL . '/datastream/OBJ/download/' . $label   ;
			if ($info['lang'] == "es") {
				$link = '<a href="' . $info['streamURL'] . '">Descarga ' . $label . " Documento</a><br />" ;					
			} else {
				$link = '<a href="' . $info['streamURL'] . '">Download ' . $label . " Document</a>" ;
			}
			$output .= $link . "<br />";
			
			break;
		case 'info:fedora/islandora_sp_basic_image':
			$info['objType'] = "Image";
//			$info['streamURL'] = $currentURL . '/datastream/MEDIUM_SIZE/download/' . $label    ;
			$info['objURL'] = $currentURL . '/datastream/OBJ/download/' . $label   ;
			if ($info['lang'] == "es") {
				$link = '<a href="' . $info['objURL'] . '">Descargar Image</a><br />' ;					
			} else {
				$link = '<a href="' . $info['objURL'] . '">Download Image</a>' ;
			}
			$output .= $link . "<br />";
			break;
		case 'info:fedora/islandora_sp_large_image_cmodel':
			$info['objType'] = "Image";
			$info['streamURL'] = $currentURL . '/datastream/JPG/download/' . $label    ;
			$info['objURL'] = $currentURL . '/datastream/OBJ/download/' . $label   ;
			
			if ($info['lang'] == "es") {
				$link = '<a href="' . $info['objURL'] . '">Descargar imagen grande</a><br />' ;	
//				$link .= '<a href="' . $info['streamURL'] . '" download="' . $label . '">Descargar Medium Image</a>' ;					
			} else {
				$link = '<a href="' . $info['objURL'] . '">Download Large Image</a><br />' ;
//				$link .= '<a href="' . $info['streamURL'] . '" download="' . $label . '">Download Meduim Image</a>' ;
			}
			$output .= $link . "<br />";
			break;
		case 'info:fedora/islandora_binaryObjectCModel':
			$info['objType'] = "Binary";
			$info['objURL'] = $currentURL . '/datastream/OBJ/download/' . $label   ;
			
			if ($info['lang'] == "es") {
				$link = '<a href="' . $info['objURL'] . '">Descarga ' . $label . " </a><br />" ;					
			} else {
//				$link = '<a href="' . $info['objURL'] . '" download="' . $label . '">Download ' . $label . " </a>" ;
				$link = "" ;
			}
			$output .= $link . "<br />";
			break;
		default:
			$info['objType'] = "other";
			
			break;
	}
		
	$dsOBJ = $object['OBJ'];
	if (!($dsOBJ)) { $output = "" ; }
	return $output;
}

