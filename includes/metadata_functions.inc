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

function parseResourceDatastreamMODS($content) {
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
			$langPIDs[] = $langPID;
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
					$roles_eng .= $r->roleTerm . ', ';
				}
				if ($r->roleTerm[@lang] == 'spa') {
					$roles_spa .= $r->roleTerm . ', ';
				}
			}
			$roles_eng = preg_replace('/, $/', '', $roles_eng);
			$roles_spa = preg_replace('/, $/', '', $roles_spa);
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
//	$info['eng']['Country(ies) (PID)'] = (array)$countryPIDs;
	$info['eng']['Subject Language'] = (array)$languages;
	$info['eng']['Language PID(s)'] = (array)$langPIDs;
	$info['eng']['Title [Indigenous]'] = (string)$alt_title_indigenous;
	$info['eng']['Title'] = (string)$title_eng;
	$info['eng']['Language Community'] = (string)$lang_comm;
	$info['eng']['Country(ies)'] = (array)$countries;
	$info['eng']['Place Created'] = (string)$place_created;
	$info['eng']['Date Created'] = (string)$date_created;
	$info['eng']['Description [Indigenous]'] = (string)$description_ind;
	$info['eng']['Language of Indigenous Description'] = (string)$description_lang;
	$info['eng']['Description'] = (string)$description_eng;
	$info['eng']['Genres'] = (array)$genres_eng;
	$info['eng']['Source Note'] = (string)$source_note;
	$info['eng']['References'] = (array) $references;
	$info['eng']['Contributor(s) Individual / Role'] = (array)$contributors_personal_eng;
	$info['eng']['Contributor(s) Corporate / Role'] = (array)$contributors_corp_eng;

	$info['spa']['Idioma'] = (array)$languages;
	$info['spa']['Idioma PID(s)'] = (array)$langPIDs;
	$info['spa']['Título [Indígena]'] = (string)$alt_title_indigenous;
	$info['spa']['Título'] = (string)$title_spa;
	$info['spa']['Comunidad del Idioma'] = (string)$lang_comm;
	$info['spa']['Pais'] = (array)$countries;
	$info['spa']['Lugar de Creación'] = (string)$place_created;
	$info['spa']['Fecha de Creación'] = (string)$date_created;
	$info['spa']['Descripción [Indígena]'] = (string)$description_ind;
	$info['spa']['Language of Indigenous Description'] = (string)$description_lang;
	$info['spa']['Descripción'] = (string)$description_spa;
	$info['spa']['Géneros'] = (array)$genres_spa;
	$info['spa']['Nota Sobre la Fuente'] = (string)$source_note;
	$info['spa']['Referencia'] = (array) $references;
	$info['spa']['Contribuidor(es) Individual / Rol de colaborador'] = (array)$contributors_personal_spa;
	$info['spa']['Contribuidor(es) Corporativo / Rol de colaborador'] = (array)$contributors_corp_spa;

	return $info;
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
	$info['eng']['References'] = (array) $references;

	$info['spa']['Idioma'] = (array)$languages;
	$info['spa']['Título [Indígena]'] = (string)$alt_title_indigenous;
	$info['spa']['Título'] = (string)$title_spa;
	$info['spa']['Coleccionista'] = (array)$collectors_spa;
	$info['spa']['Depositante'] = (array)$depositors_spa;
	$info['spa']['Descripción [Indígena]'] = (string)$description_ind;
	$info['spa']['Language of Indigenous Description'] = (string)$description_lang;
	$info['spa']['Descripción'] = (string)$description_spa;
	$info['spa']['Referencia'] = (array) $references;

	return $info;
}

function parseAudioDatastreamMODS($content) {

	/* Set Vars */
	$info = array();
	$languagePIDs = array();
	$languages = array();
	$content_type_eng = '';
	$content_type_spa = '';
	$date_created = '';
	$date_archived = '';
	$tech_description_eng = '';
	$tech_description_spa = '';
	$entered_by = '';
	$length = '';
	$encoding = '';
	$platform = '';
	$org_medium_eng = '';
	$org_medium_spa = '';
	$quality = '';
	
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
	
	/* Data from genre (creates Content Type) */	
	if (!empty($mods->genre)) {
		$i = 0;
		foreach ($mods->genre as $genre) {
			if ($genre[@displayLabel] == 'Content type (English)') {
				$content_type_eng[$i] = (string)$genre;
			$i++;
			continue;
			}
			if ($genre[@displayLabel] == 'Content type (Spanish)') {
				$content_type_spa[$i] = (string)$genre;
			$i++;
			continue;
			}
		}
	}
	/* End Data from genre */	
	
	/* Data from originInfo (creates date info) */	
	if (!empty($mods->originInfo)) {
		foreach ($mods->originInfo as $origin) {
			if ($origin->dateCreated[@displayLabel] == 'Date research material was created') {
				$date_created = (string)$origin->dateCreated;
			}
			if ($origin->dateOther[@displayLabel] == 'Date archived') {
				$date_archived = (string)$origin->dateOther;
			}
		}
	}
	/* End Data from originInfo */	

	/* Data from Note (Tech description & entered by) */
	if (!empty($mods->note)) {
		foreach ($mods->note as $note) {
			if ((string)$note[@displayLabel] == 'Technical description (English)') {
				$tech_description_eng = (string)$note;
			}
			if ((string)$note[@displayLabel] == 'Technical description (Spanish)') {
				$tech_description_spa = (string)$note;
			}
			if ((string)$note[@type] == 'record producer') {
				$entered_by = (string)$note;
			}
		}
	}
	/* Data from Note (Language Community & Source Note) */

	/* Data from physicalDescription */
	if (!empty($mods->physicalDescription)) {
		$length = (string)$mods->physicalDescription->extent;
		foreach ($mods->physicalDescription->note as $phys) {
			if ((string)$phys[@type] == 'encoding specifications') {
				$encoding = (string)$phys;
			}
			if ((string)$phys[@type] == 'platform') {
				$platform = (string)$phys;
			}
			if ((string)$phys[@type] == 'original quality') {
				$quality = (string)$phys;
			}
		}
		foreach ($mods->physicalDescription->form as $physForm) {
			if ((string)$physForm[@lang] == 'eng') {
				$org_medium_eng = (string)$physForm;
			}
			if ((string)$physForm[@lang] == 'spa') {
				$org_medium_spa = (string)$physForm;
			}
		}
	}
	/* Data from Note (Language Community & Source Note) */

	/* Package data for return to view */
	$info['eng']['Language(s)'] = (array)$languages;
	$info['eng']['Language PID(s)'] = (array)$languagePIDs;
	$info['eng']['Content type'] = (array)$content_type_eng;
	$info['eng']['Date Created'] = (string)$date_created;
	$info['eng']['Date Archived'] = (string)$date_archived;
	$info['eng']['Technical Description'] = (string)$tech_description_eng;
	$info['eng']['Length'] = (string)$length;
	$info['eng']['Encoding Specifications'] = (string)$encoding;
	$info['eng']['Platform'] = (string)$platform;
	$info['eng']['Original Medium'] = (string)$org_medium_eng;
	$info['eng']['Quality of Original Medium'] = (string)$quality;
	$info['eng']['Entered By'] = (string)$entered_by;

	$info['spa']['Idioma(s)'] = (array)$languages;
	$info['spa']['Idioma PID(s)'] = (array)$languagePIDs;
	$info['spa']['Tipo de contenido'] = (array)$content_type_spa;
	$info['spa']['Fecha de Creación'] = (string)$date_created;
	$info['spa']['Fecha archivada'] = (string)$date_archived;
	$info['spa']['Descripción técnica'] = (string)$tech_description_spa;
	$info['spa']['Duración'] = (string)$length;
	$info['spa']['Codificación'] = (string)$encoding;
	$info['spa']['Platforma'] = (string)$platform;
	$info['spa']['Medio original'] = (string)$org_medium_spa;
	$info['spa']['Calidad del medio original'] = (string)$quality;
	$info['spa']['Capturista'] = (string)$entered_by;
	
	return $info;
}

function parseVideoDatastreamMODS($content) {

	$info = array();
	$languagePIDs = array();
	$languages = array();
	$content_type_eng = '';
	$content_type_spa = '';
	$date_created = '';
	$date_archived = '';
	$tech_description_eng = '';
	$tech_description_spa = '';
	$entered_by = '';
	$length = '';
	$encoding = '';
	$platform = '';
	$org_medium_eng = '';
	$org_medium_spa = '';
	$quality = '';
	
	$xml = (string)$content;
	
	$mods = simplexml_load_string($xml);

	/* Data from LanguageTerm */	
	if (!empty($mods->language->languageTerm[@valueURI])) {
		$i = 0;
		foreach ($mods->language as $l) {
			if (!empty($l->languageTerm[@valueURI])) {
				$languagePIDs[$i] = (string)$l->languageTerm[@valueURI];
				$i++;
			}
		}
		
		foreach ($languagePIDs as $lang) {
			$lang = explode('/', $lang);
			$langPID = array_pop($lang);
			$languages[] = utailla_get_title($langPID);
		}
	}
	/* End Data from LanguageTerm */	
	
	/* Data from genre (creates Content Type) */	
	if (!empty($mods->genre)) {
		$i = 0;
		foreach ($mods->genre as $genre) {
			if ($genre[@displayLabel] == 'Content type (English)') {
				$content_type_eng[$i] = (string)$genre;
			$i++;
			continue;
			}
			if ($genre[@displayLabel] == 'Content type (Spanish)') {
				$content_type_spa[$i] = (string)$genre;
			$i++;
			continue;
			}
		}
	}
	/* End Data from genre */	
	
	/* Data from originInfo (creates date info) */	
	if (!empty($mods->originInfo)) {
		foreach ($mods->originInfo as $origin) {
			if ($origin->dateCreated[@displayLabel] == 'Date research material was created') {
				$date_created = (string)$origin->dateCreated;
			}
			if ($origin->dateOther[@displayLabel] == 'Date archived') {
				$date_archived = (string)$origin->dateOther;
			}
		}
	}
	/* End Data from originInfo (creates date info)  */	

	/* Data from Note (Tech description & entered by) */
	if (!empty($mods->note)) {
		foreach ($mods->note as $note) {
			if ((string)$note[@displayLabel] == 'Technical description (English)') {
				$tech_description_eng = (string)$note;
			}
			if ((string)$note[@displayLabel] == 'Technical description (Spanish)') {
				$tech_description_spa = (string)$note;
			}
			if ((string)$note[@type] == 'record producer') {
				$entered_by = (string)$note;
			}
		}
	}
	/* Data from Note (Tech description & entered by) */

	/* Data from physicalDescription */
	if (!empty($mods->physicalDescription)) {
		$length = (string)$mods->physicalDescription->extent;
		foreach ($mods->physicalDescription->note as $phys) {
			if ((string)$phys[@type] == 'platform') {
				$platform = (string)$phys;
			}
			if ((string)$phys[@type] == 'original quality') {
				$quality = (string)$phys;
			}
		}
		foreach ($mods->physicalDescription->form as $physForm) {
			if ((string)$physForm[@lang] == 'eng') {
				$org_medium_eng = (string)$physForm;
			}
			if ((string)$physForm[@lang] == 'spa') {
				$org_medium_spa = (string)$physForm;
			}
		}
	}
	/* Data from Note (Language Community & Source Note) */

	/* Package data for return to view */
	$info['eng']['Language(s)'] = (array)$languages;
	$info['eng']['Language PID(s)'] = (array)$languagePIDs;
	$info['eng']['Content type'] = (array)$content_type_eng;
	$info['eng']['Date Created'] = (string)$date_created;
	$info['eng']['Date Archived'] = (string)$date_archived;
	$info['eng']['Technical Description'] = (string)$tech_description_eng;
	$info['eng']['Length'] = (string)$length;
	$info['eng']['Platform'] = (string)$platform;
	$info['eng']['Original Medium'] = (string)$org_medium_eng;
	$info['eng']['Quality of Original Medium'] = (string)$quality;
	$info['eng']['Entered By'] = (string)$entered_by;

	$info['spa']['Idioma(s)'] = (array)$languages;
	$info['spa']['Idioma PID(s)'] = (array)$languagePIDs;
	$info['spa']['Tipo de contenido'] = (array)$content_type_spa;
	$info['spa']['Fecha de Creación'] = (string)$date_created;
	$info['spa']['Fecha archivada'] = (string)$date_archived;
	$info['spa']['Descripción técnica'] = (string)$tech_description_spa;
	$info['spa']['Duración'] = (string)$length;
	$info['spa']['Platforma'] = (string)$platform;
	$info['spa']['Medio original'] = (string)$org_medium_spa;
	$info['spa']['Calidad del medio original'] = (string)$quality;
	$info['spa']['Capturista'] = (string)$entered_by;

	return $info;
}

function parseBinDocPDFDatastreamMODS($content) {

	$info = array();
	$languagePIDs = array();
	$languages = array();
	$content_type_eng = '';
	$content_type_spa = '';
	$date_created = '';
	$date_archived = '';
	$tech_description_eng = '';
	$tech_description_spa = '';
	$entered_by = '';
	$length = '';
	$encoding = '';
	$platform = '';
	$org_medium_eng = '';
	$org_medium_spa = '';
	$quality = '';
	
	$xml = (string)$content;
	
	$mods = simplexml_load_string($xml);

	/* Data from LanguageTerm */	
	if (!empty($mods->language->languageTerm[@valueURI])) {
		$i = 0;
		foreach ($mods->language as $l) {
			if (!empty($l->languageTerm[@valueURI])) {
				$languagePIDs[$i] = (string)$l->languageTerm[@valueURI];
				$i++;
			}
		}
		
		foreach ($languagePIDs as $lang) {
			$lang = explode('/', $lang);
			$langPID = array_pop($lang);
			$languages[] = utailla_get_title($langPID);
		}
	}
	/* End Data from LanguageTerm */	
	
	/* Data from genre (creates Content Type) */	
	if (!empty($mods->genre)) {
		$i = 0;
		foreach ($mods->genre as $genre) {
			if ($genre[@displayLabel] == 'Content type (English)') {
				$content_type_eng[$i] = (string)$genre;
			$i++;
			continue;
			}
			if ($genre[@displayLabel] == 'Content type (Spanish)') {
				$content_type_spa[$i] = (string)$genre;
			$i++;
			continue;
			}
		}
	}
	/* End Data from genre */	
	
	/* Data from originInfo (creates date info) */	
	if (!empty($mods->originInfo)) {
		foreach ($mods->originInfo as $origin) {
			if ($origin->dateCreated[@displayLabel] == 'Date research material was created') {
				$date_created = (string)$origin->dateCreated;
			}
			if ($origin->dateOther[@displayLabel] == 'Date archived') {
				$date_archived = (string)$origin->dateOther;
			}
		}
	}
	/* End Data from originInfo (creates date info)  */	

	/* Data from Note (Tech description & entered by) */
	if (!empty($mods->note)) {
		foreach ($mods->note as $note) {
			if ((string)$note[@displayLabel] == 'Technical description (English)') {
				$tech_description_eng = (string)$note;
			}
			if ((string)$note[@displayLabel] == 'Technical description (Spanish)') {
				$tech_description_spa = (string)$note;
			}
			if ((string)$note[@type] == 'record producer') {
				$entered_by = (string)$note;
			}
		}
	}
	/* Data from Note (Tech description & entered by) */

	/* Data from physicalDescription */
	if (!empty($mods->physicalDescription)) {
		$length = (string)$mods->physicalDescription->extent;
		foreach ($mods->physicalDescription->note as $phys) {
			if ((string)$phys[@type] == 'platform') {
				$platform = (string)$phys;
			}
			if ((string)$phys[@type] == 'original quality') {
				$quality = (string)$phys;
			}
		}
		foreach ($mods->physicalDescription->form as $physForm) {
			if ((string)$physForm[@lang] == 'eng') {
				$org_medium_eng = (string)$physForm;
			}
			if ((string)$physForm[@lang] == 'spa') {
				$org_medium_spa = (string)$physForm;
			}
		}
	}
	/* Data from Note (Language Community & Source Note) */

	/* Package data for return to view */
	$info['eng']['Language(s)'] = (array)$languages;
	$info['eng']['Language PID(s)'] = (array)$languagePIDs;
	$info['eng']['Content type'] = (array)$content_type_eng;
	$info['eng']['Date Created'] = (string)$date_created;
	$info['eng']['Date Archived'] = (string)$date_archived;
	$info['eng']['Technical Description'] = (string)$tech_description_eng;
	$info['eng']['Length'] = (string)$length;
	$info['eng']['Platform'] = (string)$platform;
	$info['eng']['Original Medium'] = (string)$org_medium_eng;
	$info['eng']['Quality of Original Medium'] = (string)$quality;
	$info['eng']['Entered By'] = (string)$entered_by;

	$info['spa']['Idioma(s)'] = (array)$languages;
	$info['spa']['Idioma PID(s)'] = (array)$languagePIDs;
	$info['spa']['Tipo de contenido'] = (array)$content_type_spa;
	$info['spa']['Fecha de Creación'] = (string)$date_created;
	$info['spa']['Fecha archivada'] = (string)$date_archived;
	$info['spa']['Descripción técnica'] = (string)$tech_description_spa;
	$info['spa']['Duración'] = (string)$length;
	$info['spa']['Platforma'] = (string)$platform;
	$info['spa']['Medio original'] = (string)$org_medium_spa;
	$info['spa']['Calidad del medio original'] = (string)$quality;
	$info['spa']['Capturista'] = (string)$entered_by;

	return $info;
}

function parseImageDatastreamMODS($content) {

	$info = array();
	$languagePIDs = array();
	$languages = array();
	$content_type_eng = '';
	$content_type_spa = '';
	$date_created = '';
	$date_archived = '';
	$tech_description_eng = '';
	$tech_description_spa = '';
	$entered_by = '';
	$length = '';
	$encoding = '';
	$platform = '';
	$org_medium_eng = '';
	$org_medium_spa = '';
	$quality = '';
	
	$xml = (string)$content;
	
	$mods = simplexml_load_string($xml);

	/* Data from LanguageTerm */	
	if (!empty($mods->language->languageTerm[@valueURI])) {
		$i = 0;
		foreach ($mods->language as $l) {
			if (!empty($l->languageTerm[@valueURI])) {
				$languagePIDs[$i] = (string)$l->languageTerm[@valueURI];
				$i++;
			}
		}
		
		foreach ($languagePIDs as $lang) {
			$lang = explode('/', $lang);
			$langPID = array_pop($lang);
			$languages[] = utailla_get_title($langPID);
		}
	}
	/* End Data from LanguageTerm */	
	
	/* Data from genre (creates Content Type) */	
	if (!empty($mods->genre)) {
		$i = 0;
		foreach ($mods->genre as $genre) {
			if ($genre[@displayLabel] == 'Content type (English)') {
				$content_type_eng[$i] = (string)$genre;
			$i++;
			continue;
			}
			if ($genre[@displayLabel] == 'Content type (Spanish)') {
				$content_type_spa[$i] = (string)$genre;
			$i++;
			continue;
			}
		}
	}
	/* End Data from genre */	
	
	/* Data from originInfo (creates date info) */	
	if (!empty($mods->originInfo)) {
		foreach ($mods->originInfo as $origin) {
			if ($origin->dateCreated[@displayLabel] == 'Date research material was created') {
				$date_created = (string)$origin->dateCreated;
			}
			if ($origin->dateOther[@displayLabel] == 'Date archived') {
				$date_archived = (string)$origin->dateOther;
			}
		}
	}
	/* End Data from originInfo (creates date info)  */	

	/* Data from Note (Tech description & entered by) */
	if (!empty($mods->note)) {
		foreach ($mods->note as $note) {
			if ((string)$note[@displayLabel] == 'Technical description (English)') {
				$tech_description_eng = (string)$note;
			}
			if ((string)$note[@displayLabel] == 'Technical description (Spanish)') {
				$tech_description_spa = (string)$note;
			}
			if ((string)$note[@type] == 'record producer') {
				$entered_by = (string)$note;
			}
		}
	}
	/* Data from Note (Tech description & entered by) */

	/* Data from physicalDescription */
	if (!empty($mods->physicalDescription)) {
		$length = (string)$mods->physicalDescription->extent;
		foreach ($mods->physicalDescription->note as $phys) {
			if ((string)$phys[@type] == 'platform') {
				$platform = (string)$phys;
			}
			if ((string)$phys[@type] == 'original quality') {
				$quality = (string)$phys;
			}
		}
		foreach ($mods->physicalDescription->form as $physForm) {
			if ((string)$physForm[@lang] == 'eng') {
				$org_medium_eng = (string)$physForm;
			}
			if ((string)$physForm[@lang] == 'spa') {
				$org_medium_spa = (string)$physForm;
			}
		}
	}
	/* Data from Note (Language Community & Source Note) */

	/* Package data for return to view */
	$info['eng']['Language(s)'] = (array)$languages;
	$info['eng']['Language PID(s)'] = (array)$languagePIDs;
	$info['eng']['Content type'] = (array)$content_type_eng;
	$info['eng']['Date Created'] = (string)$date_created;
	$info['eng']['Date Archived'] = (string)$date_archived;
	$info['eng']['Technical Description'] = (string)$tech_description_eng;
	$info['eng']['Length'] = (string)$length;
	$info['eng']['Platform'] = (string)$platform;
	$info['eng']['Original Medium'] = (string)$org_medium_eng;
	$info['eng']['Quality of Original Medium'] = (string)$quality;
	$info['eng']['Entered By'] = (string)$entered_by;

	$info['spa']['Idioma(s)'] = (array)$languages;
	$info['spa']['Idioma PID(s)'] = (array)$languagePIDs;
	$info['spa']['Tipo de contenido'] = (array)$content_type_spa;
	$info['spa']['Fecha de Creación'] = (string)$date_created;
	$info['spa']['Fecha archivada'] = (string)$date_archived;
	$info['spa']['Descripción técnica'] = (string)$tech_description_spa;
	$info['spa']['Duración'] = (string)$length;
	$info['spa']['Platforma'] = (string)$platform;
	$info['spa']['Medio original'] = (string)$org_medium_spa;
	$info['spa']['Calidad del medio original'] = (string)$quality;
	$info['spa']['Capturista'] = (string)$entered_by;

	return $info;
}

function parseLanguageDatastream($content) {

	$info = array();
	$name_eng = '';
	$name_spa = '';
	$name_ind = '';
	$name_alt = '';
	$langCode = '';
	$langFam = '';
	$macroLang = '';
	$description_eng = '';
	$description_spa = '';
	$countries = '';
	$langPID = '';

	$xmlDoc = (string)$content;
	
	$xml = simplexml_load_string($xmlDoc);
	
	if (!empty($xml->aillaEngName)) {
		$name_eng = (string)$xml->aillaEngName;
	}
	
	if (!empty($xml->aillaSpaName)) {
		$name_spa = (string)$xml->aillaSpaName;
	}
	
	if (!empty($xml->aillaIndName)) {
		$name_ind = (string)$xml->aillaEngName;
	}
	
	if (!empty($xml->aillaAltName)) {
		$name_alt = (string)$xml->aillaAltName;
	}
	
	if (!empty($xml->aillaLangCode)) {
		$langCode = (string)$xml->aillaLangCode;
	}
	
	if (!empty($xml->aillaMacroLang)) {
		$macroLang = (string)$xml->aillaMacroLang;
	}
	
	if (!empty($xml->aillaLangFam)) {
		$langFam = (string)$xml->aillaLangFam;
	}
	
	if (!empty($xml->aillaEngDesc)) {
		$description_eng = (string)$xml->aillaEngDesc;
	}
	
	if (!empty($xml->aillaSpaDesc)) {
		$description_spa = (string)$xml->aillaSpaDesc;
	}
	
	if (!empty($xml->aillaSpokenCountries)) {
		$countries = (string)$xml->aillaSpokenCountries;
	}
	
	if (!empty($xml->recordInfo)) {
		$langPID = (string)$xml->recordInfo->recordIdentifier;
	}

	$info['eng']['Name'] = (string)$name_eng;
	$info['eng']['Name (Indigenous)'] = (string)$name_ind;
	$info['eng']['Alternate Name(s)'] = (string)$name_alt;
	$info['eng']['Language Code'] = (string)$langCode;
	$info['eng']['Macro Language'] = (string)$macroLang;
	$info['eng']['Language Family'] = (string)$langFam;
	$info['eng']['Description'] = (string)$description_eng;
	$info['eng']['Countries'] = (string)$countries;
	$info['eng']['Language PID'] = (string)$langPID;
	
	$info['spa']['Nombre'] = (string)$name_spa;
	$info['spa']['Nombre Indígena'] = (string)$name_ind;
	$info['spa']['Nombre Alternativo'] = (string)$name_alt;
	$info['spa']['Código de lengua'] = (string)$langCode;
	$info['spa']['MacroLengua'] = (string)$macroLang;
	$info['spa']['Familia de Lengua'] = (string)$langFam;
	$info['spa']['Descripción'] = (string)$description_spa;
	$info['spa']['Pais'] = (string)$countries;
	$info['spa']['Idioma PID'] = (string)$langPID;

	return $info;
}

function parseOrganizationDatastream($content) {

	$info = array();
	$orgName = '';
	$acronym = '';
	$directors = array();
	$funder = array();
	$parentInstitution = '';
	$description_eng = '';
	$description_spa = '';
	$depositorStatus = '';
	$researchLanguages = array();

	$madsDoc = (string)$content;
	
	$mads = simplexml_load_string($madsDoc);
	
	if (!empty($mads->authority->name)) {
		$orgName = (string)$mads->authority->name->namePart;
	}
	
	if (!empty($mads->note)) {
			$i = 0;
		foreach ($mads->note as $note) {
			if ((string)$note[@type] == 'acronym') {
				$acronym = (string)$note;
			}
			if ((string)$note[@type] == 'director') {
				$directors[$i] = (string)$note;
				$i++;
				continue;
			}
			if ((string)$note[@type] == 'funder') {
				$funder[$i] = (string)$note;
				$i++;
				continue;
			}
			if ((string)$note[@type] == 'parent institution') {
				$parentInstitution = (string)$note;
			}
			if ((string)$note[@type] == 'biographical/historical') {
				if ((string)$note[@lang] == 'eng') {
					$description_eng = (string)$note;
				}
				if ((string)$note[@lang] == 'spa') {
					$description_spa = (string)$note;
				}
			}
			if ((string)$note[@type] == 'depositor status') {
				$depositorStatus = (string)$note;
			}
		}
	}
	
	if (!empty($mads->fieldOfActivity[@valueURI])) {
		$i = 0;
		foreach ($mads->fieldOfActivity as $l) {
			if (!empty($l[@valueURI])) {
				$rLangPIDs[$i] = (string)$l[@valueURI];
				$i++;
			}
		}
		
		foreach ($rLangPIDs as $lang) {
			$lang = explode('/', $lang);
			$langPID = array_pop($lang);
			$researchLanguages[] = utailla_get_title($langPID);
		}
	}
	
	$info['eng']['Organization Name'] = (string)$orgName;
	$info['eng']['Acronym'] = (string)$acronym;
	$info['eng']['Director(s) Name(s)'] = (array)$directors;
	$info['eng']['Funder'] = (array)$funder;
	$info['eng']['Parent Institution'] = (string)$parentInstitution;
	$info['eng']['Description'] = (string)$description_eng;
	$info['eng']['Depositor Status'] = (string)$depositorStatus;
	$info['eng']['Research Language(s)'] = (array)$researchLanguages;
	
	$info['spa']['Nombre de la Organización'] = (string)$orgName;
	$info['spa']['Acrónimo'] = (string)$acronym;
	$info['spa']['Nombre(s) Completo(s) de el(los) Director(es)'] = (array)$directors;
	$info['spa']['Proveedor de Fondos'] = (array)$funder;
	$info['spa']['Entidad Matriz'] = (string)$parentInstitution;
	$info['spa']['Descripción'] = (string)$description_spa;
	$info['spa']['Depositante'] = (string)$depositorStatus;
	$info['spa']['Idioma(s) de Investigación'] = (array)$researchLanguages;
	
	return $info;
	
}

function parseCountryDatastream($content) {

	$info = array();
	$name_eng = '';
	$name_spa = '';
	$countryCode = '';
	$viaf = '';
	
	$madsDoc = (string)$content;
	
	$mads = simplexml_load_string($madsDoc);
	
	if (!empty($mads->authority->geographic)) {
		foreach($mads->authority->geographic as $geo) {
			if ((string)$geo[@lang] == 'eng') {
				$name_eng = (string)$geo;
			}
			if ((string)$geo[@lang] == 'spa') {
				$name_spa = (string)$geo;
			}
			if ((string)$geo[@authority] == 'viaf') {
				$viaf = (string)$geo;
			}
			if (((string)$geo[@authority] == 'iso3166-1') && ((string)$geo[@lang] != 'eng')) {
				$countryCode = (string)$geo;
			}
		}
	}


	$info['eng']['Name (English)'] = $name_eng;
	$info['eng']['Name (Spanish)'] = $name_spa;
	$info['eng']['Country Code'] = $countryCode;
	$info['eng']['VIAF Permalink'] = $viaf;
	
	$info['spa']['Nombre (Inglés)'] = $name_eng;
	$info['spa']['Nombre (Español)'] = $name_spa;
	$info['spa']['Código del País'] = $countryCode;
	$info['spa']['Enlace Permanente de VIAF'] = $viaf;

	return $info;
	
}

function parsePersonsDatastream($content) {

	$info = array();
	$givenNames = array();
	$surnames = array();
	$sex = '';
	$placeOfOrigin = '';
	$description_eng = '';
	$description_spa = '';
	$depositorStatus = '';
	$nativeLanguages = array();
	$researchLanguages = array();
	$otherLanguages = array();
	$organizations = array();
	
	$madsDoc = (string)$content;
	
	$mads = simplexml_load_string($madsDoc);
	
	if (!empty($mads->authority->name)) {
		$i = 0;
		foreach ($mads->authority->name->namePart as $n) {
			if ($n[@type] == 'given') {
			$givenNames[$i] = (string)$n;
			continue;
			}
			if ($n[@type] == 'family') {
			$surnames[$i] = (string)$n;
			}
			$i++;
		}
	}
	
	if (!empty($mads->note)) {
			$i = 0;
		foreach ($mads->note as $note) {
			if ((string)$note[@type] == 'biographical/historical') {
				if ((string)$note[@displayLabel] == 'Sex') {
					$sex = (string)$note;
				}
				if ((string)$note[@displayLabel] == 'Place of Origin') {
					$placeOfOrigin = (string)$note;
				}
				if ((string)$note[@lang] == 'eng') {
					$description_eng = (string)$note;
				}
				if ((string)$note[@lang] == 'spa') {
					$description_spa = (string)$note;
				}
			}
			if ((string)$note[@type] == 'depositor status') {
				$depositorStatus = (string)$note;
			}
		}
	}
	
	if (!empty($mads->fieldOfActivity[@valueURI])) {
		$i = 0;
		foreach ($mads->fieldOfActivity as $l) {
			if (!empty($l[@valueURI])) {
				if ((string)$l[@type] == 'native language') {
					$nLangPIDs[$i] = (string)$l[@valueURI];
					$i++;
				}
				if ((string)$l[@type] == 'research language') {
					$rLangPIDs[$i] = (string)$l[@valueURI];
					$i++;
				}
				if ((string)$l[@type] == 'other language') {
					$oLangPIDs[$i] = (string)$l[@valueURI];
					$i++;
				}
			}
		}
		
		foreach ($nLangPIDs as $lang) {
			$lang = explode('/', $lang);
			$langPID = array_pop($lang);
			$nativeLanguages[] = utailla_get_title($langPID);
		}
		foreach ($rLangPIDs as $lang) {
			$lang = explode('/', $lang);
			$langPID = array_pop($lang);
			$researchLanguages[] = utailla_get_title($langPID);
		}
		foreach ($oLangPIDs as $lang) {
			$lang = explode('/', $lang);
			$langPID = array_pop($lang);
			$otherLanguages[] = utailla_get_title($langPID);
		}
	}

	if (!empty($mads->affiliation->organization[@valueURI])) {
		$i = 0;
		foreach ($mads->affiliation->organization as $org) {
			if (!empty($org[@valueURI])) {
				$orgPIDs[$i] = (string)$org[@valueURI];
				$i++;
			}
		}
		foreach ($orgPIDs as $o) {
			$o = explode('/', $o);
			$orgPID = array_pop($o);
			$organizations[] = utailla_get_title($orgPID);
		}
	}

	$info['eng']['Given Name(s)'] = (array)$givenNames;
	$info['eng']['Surname(s)'] = (array)$surnames;
	$info['eng']['Sex'] = (string)$sex;
	$info['eng']['Place of Origin'] = (string)$placeOfOrigin;
	$info['eng']['Description'] = (string)$description_eng;
	$info['eng']['Depositor Status'] = (string)$depositorStatus;
	$info['eng']['Native Language(s)'] = (array)$nativeLanguages;
	$info['eng']['Research Language(s)'] = (array)$researchLanguages;
	$info['eng']['Other Language(s) Spoken'] = (array)$otherLanguages;
	$info['eng']['Organization(s)'] = (array)$organizations;
	
	$info['spa']['Nombres dados'] = (array)$givenNames;
	$info['spa']['Apellido(s)'] = (array)$surnames;
	$info['spa']['Sexo'] = (string)$sex;
	$info['spa']['Lugar de origen'] = (string)$placeOfOrigin;
	$info['spa']['Descripción'] = (string)$description_spa;
	$info['spa']['Depositante'] = (string)$depositorStatus;;
	$info['spa']['Lenguaje nativo'] = (array)$nativeLanguages;
	$info['spa']['Idiomas de investigación'] = (array)$researchLanguages;
	$info['spa']['Otros idiomas hablados'] = (array)$otherLanguages;
	$info['spa']['Organizaciones'] = (array)$organizations;

	return $info;
	
}