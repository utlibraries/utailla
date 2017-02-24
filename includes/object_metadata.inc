<?php

/**
* @file
* Creates an English metadata view (block) for a Resource
**/
function utailla_object_metadata()
{
    module_load_include('inc', 'utailla', 'includes/metadata_functions');
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
    
//    $header_title = utailla_get_title($objectID);
    $header_title = '';

	$relsDatastream = $object['RELS-EXT'];
	$relsResult = $relsDatastream->content;
	$relsInfo = parse_relsExt_for_Cmodel($relsResult);
	$info = array();
	
	switch ($relsInfo) {
		case 'info:fedora/ailla:resourceCModel':
			$resource_datastream = $object['MODS'];
			if ($resource_datastream) {
				$info = parseResourceDatastreamMODS($resource_datastream->content);
				$header_title = '<p /><p /><h1 class="page-title">'.$info['spa']['Título'].'</h1>';
			}
			break;
		case 'info:fedora/islandora_collectionCModel':
			$collection_datastream = $object['MODS'];
			if ($collection_datastream) {
				$info = parseCollectionDatastreamMODS($collection_datastream->content);
				$header_title = '<p /><p /><h1 class="page-title">'.$info['spa']['Título'].'</h1>';
			}
			break;
		case 'info:fedora/islandora_sp-audioCModel':
			$audio_datastream = $object['MODS'];
			if ($audio_datastream) {
				$info = parseAudioDatastreamMODS($audio_datastream->content);
			}
			break;
		case 'info:fedora/islandora_sp_pdf':
			$pdf_datastream = $object['MODS'];
			if ($pdf_datastream) {
				$info = parseBinDocPDFDatastreamMODS($pdf_datastream->content);
			}
			break;
		case 'info:fedora/islandora_sp_videoCModel':
			$video_datastream = $object['MODS'];
			if ($video_datastream) {
				$info = parseVideoDatastreamMODS($video_datastream->content);
			}
			break;
		case 'info:fedora/islandora_binaryObjectCModel':
			$binaryObject_datastream = $object['MODS'];
			if ($binaryObject_datastream) {
				$info = parseBinDocPDFDatastreamMODS($binaryObject_datastream->content);
			}
			break;
		case 'info:fedora/islandora_sp_document':
			$document_datastream = $object['MODS'];
			if ($document_datastream) {
				$info = parseBinDocPDFDatastreamMODS($document_datastream->content);
			}
			break;
		case 'info:fedora/islandora_sp_basic_image':
			$basicImage_datastream = $object['MODS'];
			if ($basicImage_datastream) {
				$info = parseImageDatastreamMODS($basicImage_datastream->content);
			}
			break;
		case 'info:fedora/islandora_sp_large_image_cmodel':
			$largeImage_datastream = $object['MODS'];
			if ($largeImage_datastream) {
				$info = parseImageDatastreamMODS($largeImage_datastream->content);
			}
			break;
		case 'info:fedora/ailla:languageCModel':
			$language_datastream = $object['LANGUAGE'];
			if ($language_datastream) {
				$info = parseLanguageDatastream($language_datastream->content);
			}
			break;
		case 'info:fedora/ailla:organizationCModel':
			$organization_datastream = $object['MADS'];
			if ($organization_datastream) {
				$info = parseOrganizationDatastream($organization_datastream->content);
			}
			break;
		case 'info:fedora/ailla:countryCModel':
			$country_datastream = $object['MADS'];
			if ($country_datastream) {
				$info = parseCountryDatastream($country_datastream->content);
			}
			break;
		case 'info:fedora/ailla:personsCModel':
			$persons_datastream = $object['MADS'];
			if ($persons_datastream) {
				$info = parsePersonsDatastream($persons_datastream->content);
//				drupal_set_message(print_r($info, true));
			}
			break;
	}
	
	if ($langSwitch == 'es') {
		$info = $info['spa'];
		$block_title = '<h2>Detalles del Objeto</h2>';
	} else {
		$info = $info['eng'];
		$block_title = '<h2>Object Details</h2>';
	}

	foreach ($info as $key =>$value) {
		if (is_array($value)) {
			foreach ($value as $v) {
				$formatted .= $v . '<br />';
			}
			$value = $formatted;
			$formatted = '';
		}
		$rows[] = array($key,$value);
	}

	if (!empty($info)) {
		$btitle = $block_title;
	} else {
		$btitle = '';
	}
//	$main_title = '<p /><p /><h1 class="page-title">'.$header_title.'</h1>';
	$header = '';
	$output = theme('table', array(
					'header' => $header,
					'rows'   => $rows,
					'attributes' => array('width' => '100%')
				));
//	$output = $btitle . $output;
//	$output = $main_title . $btitle . $output;
	$output = $header_title . $btitle . $output;
	
	return $output;
}
