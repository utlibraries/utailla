<?php
/**
 * Function to Parse Resource xml for Object Metadata Display
 *
 */
function parse_relsExt_for_Cmodel($relsResult) {
	$model = '';
	$RDF = $relsResult;

	$RDF = str_replace('rdf:', 'rdf_', $RDF);
	$RDF = str_replace('fedora:', 'fedora_', $RDF);
	$RDF = str_replace('fedora-model:', 'fedora_model_', $RDF);
	$RDF = str_replace('islandora:', 'islandora_', $RDF);

	$XML = simplexml_load_string($RDF);

	$model = $XML->rdf_Description->fedora_model_hasModel['rdf_resource'];

	return $model;
}

function utailla_get_title($PID) {
	$islandora_tuque = islandora_get_tuque_connection();
	$title = '';
	$object = $islandora_tuque->repository->getObject($PID);
	$title = $object->label;
	
	return $title;
}

function parseResourceDatastreamMODS($content, $langSwitch) {
	/* Set Vars */
	$info = array();
	$languagePIDs = array();
	$languages = array();
	$title_eng = '';
	$title_spa = '';
	$alt_title_indigenous = '';
	$description_eng = '';
	$description_spa = '';
	$description_indigenous = '';
	$lang_comm = '';
	$countryPIDS = array();
	$place_created = '';
	$date_created = '';
	$contributors_personal_eng = array();
	$contributors_personal_spa = array();
	$contributors_corp_eng = array();
	$contributors_corp_spa = array();
	$genres_eng = array();
	$genres_spa = array();
	$source_note = '';
	$references = array();
	
	$xml = (string)$content;
	
	$mods = simplexml_load_string($xml);

	/* Data from LanguageTerm */	
	if (!empty($mods->language->languageTerm[@valueURI])) {
		$i = 0;
		foreach ($mods->language as $l) {
			$languagePIDs[$i] = (string)$l->languageTerm[@valueURI];
			$i++;
		}
		
		foreach ($languagePIDs as $langP) {
			$langP = explode('/', $langP);
			$langPID = array_pop($langP);
			$languages[] = utailla_get_title($langPID);
		}
	}
	/* End Data from LanguageTerm */	
	
	/* Data from TitleInfo */
	if (!empty($mods->titleInfo->title)) {
		foreach ($mods->titleInfo as $titleInfo) {
			if ((string)$titleInfo[@type] == 'alternative') {
				$alt_title_indigenous = (string)$titleInfo->title;
			}
			if((string)$titleInfo[@lang] == 'eng') {
				$title_eng = (string)$titleInfo->title;
			}
			if((string)$titleInfo[@lang] == 'spa') {
				$title_spa = (string)$titleInfo->title;
			}
		}
	}
	/* End Data from TitleInfo */

	/* Data from Abstract (description) */
	if (!empty($mods->abstract)) {
		foreach ($mods->abstract as $abs) {
			if ((string)$abs[@lang] == 'eng') {
				$description_eng = (string)$abs;
			}
			if((string)$abs[@lang] == 'spa') {
				$description_spa = (string)$abs;
			}
			if((string)$abs[@displayLabel] == 'Resource description (Indigenous language)') {
				$description_ind = (string)$abs;
				$description_lang = (string)$abs[@lang];
			}
		}
	}
	/* End Data from Abstract (description) */
	
	/* Data from Note (Language Community & Source Note) */
	if (!empty($mods->note)) {
		foreach ($mods->note as $note) {
			if ((string)$note[@type] == 'language community') {
				$lang_comm = (string)$note;
			}
			if ((string)$note[@type] == 'source note') {
				$source_note = (string)$note;
			}
			if ((string)$note[@type] == 'citation/reference') {
				$references[] = (string)$note;
			}
		}
	}
	/* Data from Note (Language Community & Source Note) */

	/* Data from originInfo */	
	if (!empty($mods->originInfo)) {
		$i = 0;
		foreach ($mods->originInfo as $origin) {
			if ($origin->place->placeTerm[@displayLabel] == 'Country') {
				$countryPIDs[$i] = (string)$origin->place->placeTerm[@valueURI];
			}
			if ($origin->place->placeTerm[@displayLabel] == 'Place created') {
				$place_created = (string)$origin->place->placeTerm;
			}
			$i++;
			$date_created = $origin->dateCreated;
		}
		foreach ($countryPIDs as $country) {
			$country = explode('/', $country);
			$countryPID = array_pop($country);
			$countries[] = utailla_get_title($countryPID);
		}
	}
	/* End Data from originInfo */	

	/* Data from Name/Role */
    if (!empty($mods->name)) {
        $i = 0;
        $namePIDS = array();
        foreach ($mods->name as $n) {
            $namePID = (string)$n[@valueURI];
			foreach ($n->role as $r) {
				if ($r->roleTerm[@lang] == 'eng') {
					$roles_eng .= $r->roleTerm . '</br>';
				}
				if ($r->roleTerm[@lang] == 'spa') {
					$roles_spa .= $r->roleTerm . '</br>';
				}
			}
        	$namePID = explode('/', $namePID);
        	$namePID = array_pop($namePID);
        	$name = utailla_get_title($namePID);
			if($n[@type] == 'personal') {
				$contributors_personal_eng[$i] = $name .'('. $roles_eng.')';
				$contributors_personal_spa[$i] = $name .'('. $roles_spa.')';
			}
			if($n[@type] == 'corporate') {
					$contributors_corp_eng[$i] = $name .'('. $roles_eng.')';
				if (!empty($roles_spa)) {
					$contributors_corp_spa[$i] = $name .'('. $roles_spa.')';
				} else {
					$contributors_corp_spa[$i] = $name .'('. $roles_eng.')';
				}
			}
			$roles_eng = '';
			$roles_spa = '';
	        $i++;
        }
    }
	/* End Data from Name/Role */

	/* Data from genre */	
	if (!empty($mods->genre)) {
		$i = 0;
		foreach ($mods->genre as $genre) {
			if ($genre[@lang] == 'eng') {
				$genres_eng[$i] = (string)$genre;
			$i++;
			continue;
			}
			if ($genre[@lang] == 'spa') {
				$genres_spa[$i] = (string)$genre;
			$i++;
			continue;
			}
		}
	}
	/* End Data from genre */	
	
	/* Package data for return to view */
//	$info['eng']['Language PID'] = (array)$languagePIDs;
//	$info['eng']['Country(ies) (PID)'] = (array)$countryPIDs;
	$info['eng']['Subject Language'] = (array)$languages;
	$info['eng']['Title [Indigenous]'] = (string)$alt_title_indigenous;
	$info['eng']['Title'] = (string)$title_eng;
	$info['eng']['Language Community'] = (string)$lang_comm;
	$info['eng']['Country(ies)'] = (array)$countries;
	$info['eng']['Place Created'] = (string)$place_created;
	$info['eng']['Date Created'] = (string)$date_created;
	$info['eng']['Contributor(s) Individual / Role'] = (array)$contributors_personal_eng;
	$info['eng']['Contributor(s) Corporate / Role'] = (array)$contributors_corp_eng;
	$info['eng']['Description [Indigenous]'] = (string)$description_ind;
	$info['eng']['Language of Indigenous Description'] = (string)$description_lang;
	$info['eng']['Description'] = (string)$description_eng;
	$info['eng']['Genres'] = (array)$genres_eng;
	$info['eng']['Source Note'] = (string)$source_note;
	$info['eng']['References'] = (array) $references;

	$info['spa']['Idioma'] = (array)$languages;
	$info['spa']['Título [Indígena]'] = (string)$alt_title_indigenous;
	$info['spa']['Título '] = (string)$title_spa;
	$info['spa']['Comunidad del Idioma'] = (string)$lang_comm;
	$info['spa']['Pais'] = (array)$countries;
	$info['spa']['Lugar de Creación'] = (string)$place_created;
	$info['spa']['Fecha de Creación'] = (string)$date_created;
	$info['spa']['Contribuidor(es) Individual / Rol de colaborador'] = (array)$contributors_personal_spa;
	$info['spa']['Contribuidor(es) Corporativo / Rol de colaborador'] = (array)$contributors_corp_spa;
	$info['spa']['Descripción [Indígena]'] = (string)$description_ind;
	$info['spa']['Language of Indigenous Description'] = (string)$description_lang;
	$info['spa']['Descripción '] = (string)$description_spa;
	$info['spa']['Géneros'] = (array)$genres_spa;
	$info['spa']['Nota Sobre la Fuente'] = (string)$source_note;
	$info['spa']['Referencia'] = (array) $references;

	if ($langSwitch == 'es') {
		$results = $info['spa'];
	} else {
		$results = $info['eng'];
	}

	return $results;
//	return $info;
}

function parseCollectionDatastreamMODS($content) {

	/* Set Vars */
	$info = array();
	$languagePIDs = array();
	$languages = array();
	$title_eng = '';
	$title_spa = '';
	$alt_title_indigenous = '';
	$description_eng = '';
	$description_spa = '';
	$description_indigenous = '';
	$collectors_eng = array();
	$collectors_spa = array();
	$depositors_eng = array();
	$depositors_spa = array();
	$source_note = '';
	$references = array();

	$xml = (string)$content;
	
	$mods = simplexml_load_string($xml);

	/* Data from LanguageTerm */	
	if (!empty($mods->language->languageTerm[@valueURI])) {
		$i = 0;
		foreach ($mods->language as $l) {
			$languagePIDs[$i] = (string)$l->languageTerm[@valueURI];
			$i++;
		}
		
		foreach ($languagePIDs as $lang) {
			$lang = explode('/', $lang);
			$langPID = array_pop($lang);
			$languages[] = utailla_get_title($langPID);
		}
	}
	/* End Data from LanguageTerm */	

		/* Data from TitleInfo */
	if (!empty($mods->titleInfo->title)) {
		foreach ($mods->titleInfo as $titleInfo) {
			if ((string)$titleInfo[@type] == 'alternative') {
				$alt_title_indigenous = (string)$titleInfo->title;
			}
			if((string)$titleInfo[@lang] == 'eng') {
				$title_eng = (string)$titleInfo->title;
			}
			if((string)$titleInfo[@lang] == 'spa') {
				$title_spa = (string)$titleInfo->title;
			}
		}
	}
	/* End Data from TitleInfo */

	/* Data from Name/Role */
    if (!empty($mods->name)) {
        $i = 0;
        $namePIDS = array();
        foreach ($mods->name as $n) {
            $namePID = (string)$n[@valueURI];
        	$namePID = explode('/', $namePID);
        	$namePID = array_pop($namePID);
        	$name = utailla_get_title($namePID);

			foreach ($n->role as $r) {
				if ($r->roleTerm == 'Collector') {
					$collectors_eng[$i] = $name;
				}
				if ($r->roleTerm == 'Depositor') {
					$depositors_eng[$i] = $name;
				}
				if ($r->roleTerm == 'Coleccionista') {
					$collectors_spa[$i] = $name;
				}
				if ($r->roleTerm == 'Depositante') {
					$depositors_spa[$i] = $name;
				}
			}
	        $i++;
        }
    }
	/* End Data from Name/Role */

	/* Data from Abstract (description) */
	if (!empty($mods->abstract)) {
		foreach ($mods->abstract as $abs) {
			if ((string)$abs[@lang] == 'eng') {
				$description_eng = (string)$abs;
			}
			if((string)$abs[@lang] == 'spa') {
				$description_spa = (string)$abs;
			}
			if((string)$abs[@displayLabel] == 'Resource description (Indigenous language)') {
				$description_ind = (string)$abs;
				$description_lang = (string)$abs[@lang];
			}
		}
	}
	/* End Data from Abstract (description) */

	/* Data from Note (Source Note) */
	if (!empty($mods->note)) {
		foreach ($mods->note as $note) {
			if ((string)$note[@type] == 'source note') {
				$source_note = (string)$note;
			}
		}
	}
	/* Data from Note (Language Community & Source Note) */
	
	/* Data from Related Item */
	if (!empty($mods->relatedItem)) {
	$i = 0;
		foreach ($mods->relatedItem as $item) {
			if ((string)$item->note[@type] == 'citation/reference') {
				$references[$i] = (string)$item->note;
			}
			$i++;
		}
	}
	/* Data from Related Item */


	/* Package data for return to view */
//	$info['eng']['Language PID'] = (array)$languagePIDs;
//	$info['eng']['Country(ies) (PID)'] = (array)$countryPIDs;
	$info['eng']['Subject Language'] = (array)$languages;
	$info['eng']['Title [Indigenous]'] = (string)$alt_title_indigenous;
	$info['eng']['Title'] = (string)$title_eng;
	$info['eng']['Collector(s)'] = (array)$collectors_eng;
	$info['eng']['Depositor(s)'] = (array)$depositors_eng;
	$info['eng']['Description [Indigenous]'] = (string)$description_ind;
	$info['eng']['Language of Indigenous Description'] = (string)$description_lang;
	$info['eng']['Description'] = (string)$description_eng;
	$info['eng']['Source Note'] = (string)$source_note;
	$info['eng']['References'] = (array) $references;

	$info['spa']['Idioma'] = (array)$languages;
	$info['spa']['Título [Indígena]'] = (string)$alt_title_indigenous;
	$info['spa']['Título '] = (string)$title_spa;
	$info['spa']['Coleccionista'] = (array)$collectors_spa;
	$info['spa']['Depositante'] = (array)$depositors_spa;
	$info['spa']['Descripción [Indígena]'] = (string)$description_ind;
	$info['spa']['Language of Indigenous Description'] = (string)$description_lang;
	$info['spa']['Descripción '] = (string)$description_spa;
	$info['spa']['Nota Sobre la Fuente'] = (string)$source_note;
	$info['spa']['Referencia'] = (array) $references;

	return $info;
}

function parseAudioDatastreamMODS($content) {

	/* Set Vars */
	$info = array();
	$languagePIDs = array();
	$languages = array();
	$date_created = '';
	$place_created = '';
	$date_archived = '';
	$tech_description_eng = '';
	$tech_description_spa = '';
	$lang_comm = '';
	$countryPIDS = array();
	$contributors_personal_eng = array();
	$contributors_personal_spa = array();
	$contributors_corp_eng = array();
	$contributors_corp_spa = array();
	$genres_eng = array();
	$genres_spa = array();
	$source_note = '';
	$references = array();
	
	$xml = (string)$content;
	
	$mods = simplexml_load_string($xml);

	/* Data from LanguageTerm */	
	if (!empty($mods->language->languageTerm[@valueURI])) {
		$i = 0;
		foreach ($mods->language as $l) {
			$languagePIDs[$i] = (string)$l->languageTerm[@valueURI];
			$i++;
		}
		
		foreach ($languagePIDs as $lang) {
			$lang = explode('/', $lang);
			$langPID = array_pop($lang);
			$languages[] = utailla_get_title($langPID);
		}
	}
	/* End Data from LanguageTerm */	
	
	/* Data from Abstract (description) */
	if (!empty($mods->abstract)) {
		foreach ($mods->abstract as $abs) {
			if ((string)$abs[@lang] == 'eng') {
				$description_eng = (string)$abs;
			}
			if((string)$abs[@lang] == 'spa') {
				$description_spa = (string)$abs;
			}
			if((string)$abs[@displayLabel] == 'Resource description (Indigenous language)') {
				$description_ind = (string)$abs;
				$description_lang = (string)$abs[@lang];
			}
		}
	}
	/* End Data from Abstract (description) */
	
	/* Data from Note (Language Community & Source Note) */
	if (!empty($mods->note)) {
		foreach ($mods->note as $note) {
			if ((string)$note[@type] == 'language community') {
				$lang_comm = (string)$note;
			}
			if ((string)$note[@type] == 'source note') {
				$source_note = (string)$note;
			}
			if ((string)$note[@type] == 'citation/reference') {
				$references[] = (string)$note;
			}
		}
	}
	/* Data from Note (Language Community & Source Note) */

	/* Data from originInfo */	
	if (!empty($mods->originInfo)) {
		$i = 0;
		foreach ($mods->originInfo as $origin) {
			if ($origin->place->placeTerm[@displayLabel] == 'Country') {
				$countryPIDs[$i] = (string)$origin->place->placeTerm[@valueURI];
			}
			if ($origin->place->placeTerm[@displayLabel] == 'Place created') {
				$place_created = (string)$origin->place->placeTerm;
			}
			$i++;
			$date_created = $origin->dateCreated;
		}
		foreach ($countryPIDs as $country) {
			$country = explode('/', $country);
			$countryPID = array_pop($country);
			$countries[] = utailla_get_title($countryPID);
		}
	}
	/* End Data from originInfo */	

	/* Data from Name/Role */
    if (!empty($mods->name)) {
        $i = 0;
        $namePIDS = array();
        foreach ($mods->name as $n) {
            $namePID = (string)$n[@valueURI];
			foreach ($n->role as $r) {
				if ($r->roleTerm[@lang] == 'eng') {
					$roles_eng .= $r->roleTerm . '</br>';
				}
				if ($r->roleTerm[@lang] == 'spa') {
					$roles_spa .= $r->roleTerm . '</br>';
				}
			}
        	$namePID = explode('/', $namePID);
        	$namePID = array_pop($namePID);
        	$name = utailla_get_title($namePID);
			if($n[@type] == 'personal') {
				$contributors_personal_eng[$i] = $name .'('. $roles_eng.')';
				$contributors_personal_spa[$i] = $name .'('. $roles_spa.')';
			}
			if($n[@type] == 'corporate') {
					$contributors_corp_eng[$i] = $name .'('. $roles_eng.')';
				if (!empty($roles_spa)) {
					$contributors_corp_spa[$i] = $name .'('. $roles_spa.')';
				} else {
					$contributors_corp_spa[$i] = $name .'('. $roles_eng.')';
				}
			}
			$roles_eng = '';
			$roles_spa = '';
	        $i++;
        }
    }
	/* End Data from Name/Role */

	/* Data from genre */	
	if (!empty($mods->genre)) {
		$i = 0;
		foreach ($mods->genre as $genre) {
			if ($genre[@displayLabel] == 'Content type (English)') {
				$genres_eng[$i] = (string)$genre;
			$i++;
			continue;
			}
			if ($genre[@displayLabel] == 'Content type (Spanish)') {
				$genres_spa[$i] = (string)$genre;
			$i++;
			continue;
			}
		}
	}
	/* End Data from genre */	
	
	/* Package data for return to view */
//	$info['eng']['Country(ies) (PID)'] = (array)$countryPIDs;
	$info['eng']['Language(s)'] = (array)$languages;
	$info['eng']['Language PID(s)'] = (array)$languagePIDs;
	$info['eng']['Content type'] = (array)$genres_eng;
/*	$info['eng']['Language Community'] = (string)$lang_comm;
	$info['eng']['Country(ies)'] = (array)$countries;
	$info['eng']['Place Created'] = (string)$place_created;
	$info['eng']['Date Created'] = (string)$date_created;
	$info['eng']['Contributor(s) Individual / Role'] = (array)$contributors_personal_eng;
	$info['eng']['Contributor(s) Corporate / Role'] = (array)$contributors_corp_eng;
	$info['eng']['Description [Indigenous]'] = (string)$description_ind;
	$info['eng']['Language of Indigenous Description'] = (string)$description_lang;
	$info['eng']['Description'] = (string)$description_eng;
	$info['eng']['Source Note'] = (string)$source_note;
	$info['eng']['References'] = (array) $references;
*/

	$info['spa']['Idioma(s)'] = (array)$languages;
	$info['spa']['Idioma PID(s)'] = (array)$languagePIDs;
	$info['spa']['Tipo de contenido'] = (array)$genres_spa;
/*	$info['spa']['Comunidad del Idioma'] = (string)$lang_comm;
	$info['spa']['Pais'] = (array)$countries;
	$info['spa']['Lugar de Creación'] = (string)$place_created;
	$info['spa']['Fecha de Creación'] = (string)$date_created;
	$info['spa']['Contribuidor(es) Individual / Rol de colaborador'] = (array)$contributors_personal_spa;
	$info['spa']['Contribuidor(es) Corporativo / Rol de colaborador'] = (array)$contributors_corp_spa;
	$info['spa']['Descripción [Indígena]'] = (string)$description_ind;
	$info['spa']['Language of Indigenous Description'] = (string)$description_lang;
	$info['spa']['Descripción '] = (string)$description_spa;
	$info['spa']['Nota Sobre la Fuente'] = (string)$source_note;
	$info['spa']['Referencia'] = (array) $references;
*/
	
	return $info;
}

function parsePDFDatastreamMODS($content) {

	$info = array();

	return $info;
}

function parseDocumentDatastreamMODS($content) {

	$info = array();

	return $info;
}

function parseBinaryObjectDatastreamMODS($content) {

	$info = array();

	return $info;
}

function parseVideoDatastreamMODS($content) {

	$info = array();

	return $info;
}

function parseBasicImageDatastreamMODS($content) {

	$info = array();

	return $info;
}

function parseLargeImageDatastreamMODS($content) {

	$info = array();

	return $info;
}
