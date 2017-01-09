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
		$lang = 'es';
	} else if ($lang_path == 'islandora') {
		$lang = 'eng';
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
	
	switch ($relsInfo) {
		case 'info:fedora/ailla:resourceCModel':
			$resource_datastream = $object['MODS'];
			if ($resource_datastream) {
				$info = parseResourceDatastreamMODS($resource_datastream->content, $lang);
				//drupal_set_message('<pre>'.print_r($info, true).'</pre>');
			}
			$test_message = t('Object is a resource!!!');
			break;
		case 'info:fedora/islandora_collectionCModel':
			$collection_datastream = $object['MODS'];
			if ($collection_datastream) {
				$info = parseCollectionDatastreamMODS($collection_datastream->content);
				drupal_set_message('<pre>'.print_r($info, true).'</pre>');
			}
			break;
		case 'info:fedora/islandora_sp-audioCModel':
			$audio_datastream = $object['MODS'];
			if ($audio_datastream) {
				$info = parseAudioDatastreamMODS($audio_datastream->content);
				drupal_set_message('<pre>'.print_r($info, true).'</pre>');
			}
			break;
		case 'info:fedora/islandora_sp_pdf':
			$pdf_datastream = $object['MODS'];
			if ($pdf_datastream) {
				$info = parsePDFDatastreamMODS($pdf_datastream->content);
				drupal_set_message('<pre>'.print_r($info, true).'</pre>');
			}
			break;
		case 'info:fedora/islandora_sp_document':
			$document_datastream = $object['MODS'];
			if ($document_datastream) {
				$info = parseDocumentDatastreamMODS($audio_datastream->content);
				drupal_set_message('<pre>'.print_r($info, true).'</pre>');
			}
			break;
		case 'info:fedora/islandora_binaryObjectCModel':
			$binaryObject_datastream = $object['MODS'];
			if ($binaryObject_datastream) {
				$info = parseBinaryObjectDatastreamMODS($binaryObject_datastream->content);
				drupal_set_message('<pre>'.print_r($info, true).'</pre>');
			}
			break;
		case 'info:fedora/islandora_sp_videoCModel':
			$video_datastream = $object['MODS'];
			if ($video_datastream) {
				$info = parseVideoDatastreamMODS($video_datastream->content);
				drupal_set_message('<pre>'.print_r($info, true).'</pre>');
			}
			break;
		case 'info:fedora/islandora_sp_basic_image':
			$basicImage_datastream = $object['MODS'];
			if ($basicImage_datastream) {
				$info = parseBasicImageDatastreamMODS($basicImage_datastream->content);
				drupal_set_message('<pre>'.print_r($info, true).'</pre>');
			}
			break;
		case 'info:fedora/islandora_sp_large_image_cmodel':
			$largeImage_datastream = $object['MODS'];
			if ($largeImage_datastream) {
				$info = parseLargeImageDatastreamMODS($largeImage_datastream->content);
				drupal_set_message('<pre>'.print_r($info, true).'</pre>');
			}
			break;
	}

	$list = array();
	
	foreach ($info as $key =>$value) {
		if (is_array($value)) {
			foreach ($value as $v) {
				$formatted .= $v . '<br />';
			}
			$value = $formatted;
			$formatted = '';
		}
		$list[] = t($key.': '.$value);
//		$list[] = '<dt>'.$key.'</dt><dd>'.$value.'</dd>';
	}

	$render_array['object_metadata'] = array(
		'#theme' => 'item_list',  // The theme function to apply to the #items
		'#items' => $list,  // The list itself.
	);

	return $render_array;
}
