<?php
/**
 * @file
 * Media batch object class file.
 */

class UtaillaMediaBatchObject extends IslandoraBatchObject {
  protected static $modelMap = array(
    1 => 'islandora:sp-audioCModel',
    2 => 'islandora:sp-audioCModel',
    10 => 'islandora:sp_document',
    4 => 'islandora:sp_pdf',
    5 => 'islandora:sp_document',
    6 => 'islandora:sp_document',
    7 => 'islandora:sp_document',
    8 => 'islandora:sp_document',
    9 => 'islandora:sp_basic_image',
    11 => 'islandora:sp_large_image_cmodel',
    12 => 'islandora:binaryObjectCModel',
    14 => 'islandora:sp_videoCModel',
    16 => 'islandora:sp_document',
    17 => 'islandora:sp_videoCModel',
    18 => 'islandora:binaryObjectCModel',
    19 => 'islandora:binaryObjectCModel',
    23 => 'islandora:sp_videoCModel',
    24 => 'islandora:sp_large_image_cmodel',
    25 => 'islandora:binaryObjectCModel',
  );
  protected static $mimeMap = array(
    1 => 'audio/mp3',
    2 => 'audio/wav',
    10 => 'text/richtext',
    4 => 'application/pdf',
    5 => 'application/msword',
    6 => 'text/html',
    7 => 'text/plain',
    8 => 'application/Wordperfect5.1',
    9 => 'image/jpeg',
    11 => 'image/tiff',
    12 => 'text/xml',
    14 => 'video/avi',
    16 => 'application/vnd.oasis.opendocument',
    17 => 'video/quicktime',
    18 => 'text/x-eaf+xml',
    19 => 'text/x-trs+xml',
    23 => 'video/mp4',
    24 => 'image/tiff',
    25 => 'text/xml',
  );
  protected static $metaMap = array(
    'text' => array(10, 4, 5, 6, 7, 8, 12, 16, 18, 19, 25),
    'audio' => array(1, 2, 23),
    'image' => array(9, 11, 24),
    'video' => array(14, 17),
  );

  /**
   * Constructor to preserve info on the object.
   *
   * @see IslandoraBatchObject::__constructor
   */
  public function __construct(IslandoraTuque $connection, $media_file, $genres, $encodings, $platforms, $mediums, $preprocessor_parameters) {
    parent::__construct(NULL, $connection->repository);
    $this->mediaInfo = $media_file;
    $this->genres = $genres;
    $this->encodings = $encodings;
    $this->platforms = $platforms;
    $this->mediums = $mediums;
    $this->resources = array();
    $this->preprocessorParameters = $preprocessor_parameters;
  }

  /**
   * Implement because the parent says I must.
   *
   * @see IslandoraBatchObject::getResources
   */
  public function getResources() {
    return $this->resources;
  }

  /**
   * Process the current object.
   *
   * @see IslandoraBatchObject::batchProcess
   */
  public function batchProcess() {
    module_load_include('inc', 'utailla', 'includes/migration.db');
    module_load_include('inc', 'utailla', 'includes/utilities');
    require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
    $message = '';

    $this->models = self::$modelMap[$this->mediaInfo['media_format_id']];
    $this->label = basename($this->mediaInfo['identifier']);

    $file_path = "{$this->preprocessorParameters['directory']}{$this->mediaInfo['directory_path']}{$this->mediaInfo['identifier']}";
    if (is_file($file_path)) {
      $obj = $this->constructDatastream('OBJ');
      $obj->label = $this->label;
      $obj->mimeType = self::$mimeMap[$this->mediaInfo['media_format_id']];
      $obj->setContentFromFile($file_path, FALSE);
      $this->ingestDatastream($obj);
    }
    else {
      $message = "OBJ was missing for {$this->mediaInfo['media_file_id']}.";
    }

    if (!$this->addRelationships()) {
      return array(
        'state' => ISLANDORA_BATCH_STATE__ERROR,
        'message' => "No collection available for {$this->mediaInfo['media_file_id']}.",
      );
    }

    $mods = $this->constructDatastream('MODS');
    $mods->label = 'MODS';
    $mods->mimeType = 'application/xml';
    $this->languages = utailla_get_media_file_languages($this->mediaInfo['media_file_id']);
    if (in_array($this->mediaInfo['media_format_id'], self::$metaMap['text'])) {
      $mods->content = $this->textMods();
    }
    elseif (in_array($this->mediaInfo['media_format_id'], self::$metaMap['audio'])) {
      $mods->content = $this->audioMods();
    }
    elseif (in_array($this->mediaInfo['media_format_id'], self::$metaMap['video'])) {
      $mods->content = $this->videoMods();
    }
    elseif (in_array($this->mediaInfo['media_format_id'], self::$metaMap['image'])) {
      $mods->content = $this->imageMods();
    }
    $this->ingestDatastream($mods);

    // Set XACML if necessary and update restriction information.
    // Original started at 1.
    $justification_option = $this->mediaInfo['access_justification'] - 1;
    $level = $this->mediaInfo['level'] - 1;
    if ($level != -1) {
      $restriction_info = array(
        'pid' => $this->id,
        'level' => $level,
        'end_time' => $this->mediaInfo['time_limit'] ? strtotime($this->mediaInfo['time_limit']) : NULL,
        'hint' => $this->mediaInfo['password_hint'],
        'password' => user_hash_password($this->mediaInfo['password']),
        'reason_option' => $justification_option < 0 ? NULL : $justification_option,
        'custom_reason' => $this->mediaInfo['justification_other'],
      );
      utailla_set_restriction($this, $restriction_info);
    }

    utailla_log_media_file_mapping($this->mediaInfo['media_file_id'], $this->id);
    return array(
      'state' => ISLANDORA_BATCH_STATE__DONE,
      'message' => $message,
    );
  }

  /**
   * Creates MODS for images.
   */
  protected function imageMods() {
    $writer = utailla_get_mods_writer();
    $this->writeModsResource($writer, 'still image');
    $this->writeModsCommon($writer);
    $writer->endElement();
    $writer->endDocument();
    return $writer->flush();
  }

  /**
   * Creates MODS for text.
   */
  protected function textMods() {
    $writer = utailla_get_mods_writer();
    $this->writeModsResource($writer, 'text');
    $this->writeModsCommon($writer);
    $writer->endElement();
    $writer->endDocument();
    return $writer->flush();
  }

  /**
   * Creates MODS for audio.
   */
  protected function audioMods() {
    $writer = utailla_get_mods_writer();
    $this->writeModsResource($writer, 'sound recording');
    $this->writeModsCommon($writer);
    $writer->endElement();
    $writer->endDocument();
    return $writer->flush();
  }

  /**
   * Creates MODS for videos.
   */
  protected function videoMods() {
    $writer = utailla_get_mods_writer();
    $this->writeModsResource($writer, 'moving image');
    $this->writeModsCommon($writer);
    $writer->endElement();
    $writer->endDocument();
    return $writer->flush();
  }

  /**
   * Write a typeOfResource element.
   */
  protected function writeModsResource($writer, $type) {
    $writer->startElement('typeOfResource');
    $writer->text($type);
    $writer->endElement();
  }

  /**
   * Write MODS that is common to all types.
   */
  protected function writeModsCommon($writer) {
    // Languages.
    foreach ($this->languages as $language) {
      if ($language) {
        $writer->startElement('language');
        $writer->startElement('languageTerm');
        $writer->writeAttribute('type', 'text');
        $writer->writeAttribute('authority', 'aillaLanguage');
        $writer->writeAttribute('valueURI', utailla_get_uri_from_pid($language));
        $writer->endElement();
        $writer->endElement();
      }
    }
    // Dates.
    if ($this->mediaInfo['date_created']) {
      $writer->startElement('originInfo');
      $writer->writeAttribute('eventType', 'production');
      $writer->startElement('dateCreated');
      $writer->writeAttribute('keyDate', 'yes');
      $writer->writeAttribute('displayLabel', 'Date research material was created.');
      $writer->text($this->mediaInfo['date_created']);
      $writer->endElement();
      $writer->endElement();
    }
    if ($this->mediaInfo['date_modified'] || $this->mediaInfo['date_archived']) {
      $writer->startElement('originInfo');
      $writer->writeAttribute('eventType', 'provenance');
      if ($this->mediaInfo['date_modified']) {
        $writer->startElement('dateModified');
        $writer->text($this->mediaInfo['date_modified']);
        $writer->endElement();
      }
      if ($this->mediaInfo['date_archived']) {
        $writer->startElement('dateOther');
        $writer->text($this->mediaInfo['date_archived']);
        $writer->endElement();
      }
      $writer->endElement();
    }
    // Genre.
    if ($this->mediaInfo['media_content_id']) {
      $writer->startElement('genre');
      $writer->writeAttribute('lang', 'eng');
      $writer->writeAttribute('authority', 'aillaMediaGenres');
      $writer->text($this->genres[$this->mediaInfo['media_content_id']]['content_name']);
      $writer->endElement();

      $writer->startElement('genre');
      $writer->writeAttribute('lang', 'spa');
      $writer->writeAttribute('authority', 'aillaMediaGenres');
      $writer->text($this->genres[$this->mediaInfo['media_content_id']]['contenido_nombre']);
      $writer->endElement();
    }
    // Physical description.
    $writer->startElement('physicalDescription');
    $writer->startElement('extent');
    $writer->text("{$this->mediaInfo['length_hours']}:{$this->mediaInfo['length_mins']}:{$this->mediaInfo['length_secs']}");
    $writer->endElement();
    $writer->startElement('note');
    $writer->writeAttribute('type', 'encoding specifications');
    $writer->text($this->encodings[$this->mediaInfo['encoding_id']]->encoding_name);
    $writer->endElement();
    $writer->startElement('note');
    $writer->writeAttribute('type', 'platform');
    $writer->text($this->platforms[$this->mediaInfo['platform_id']]->platform_name);
    $writer->endElement();
    $writer->startElement('form');
    $writer->writeAttribute('type', 'Original Medium');
    $writer->writeAttribute('lang', 'eng');
    $writer->writeAttribute('authority', 'aillaOriginalMedium');
    $writer->text($this->mediums[$this->mediaInfo['orig_medium_id']]['orig_medium_name']);
    $writer->endElement();
    $writer->startElement('form');
    $writer->writeAttribute('type', 'Original Medium');
    $writer->writeAttribute('lang', 'spa');
    $writer->writeAttribute('authority', 'aillaOriginalMedium');
    $writer->text($this->mediums[$this->mediaInfo['orig_medium_id']]['orig_medio_nombre']);
    $writer->endElement();
    if ($this->mediaInfo['orig_rec_speed']) {
      $writer->startElement('note');
      $writer->writeAttribute('type', 'original recording speed');
      $writer->text($this->mediaInfo['orig_rec_speed']);
      $writer->endElement();
    }
    $writer->startElement('note');
    $writer->writeAttribute('type', 'original quality');
    $writer->text($this->mediaInfo['recording_quality']);
    $writer->endElement();
    $writer->endElement();
    // Notes.
    if ($this->mediaInfo['produced_by']) {
      $writer->startElement('note');
      $writer->writeAttribute('type', 'record producer');
      $writer->text($this->mediaInfo['produced_by']);
      $writer->endElement();
    }
    if ($this->mediaInfo['description_en']) {
      $writer->startElement('note');
      $writer->writeAttribute('type', 'recording/production note');
      $writer->writeAttribute('lang', 'eng');
      $writer->text($this->mediaInfo['description_en']);
      $writer->endElement();
    }
    if ($this->mediaInfo['description_sp']) {
      $writer->startElement('note');
      $writer->writeAttribute('type', 'recording/production note');
      $writer->writeAttribute('lang', 'spa');
      $writer->text($this->mediaInfo['description_sp']);
      $writer->endElement();
    }
    $writer->startElement('note');
    $writer->writeAttribute('type', 'archival object');
    $writer->text($this->mediaInfo['archival_object']);
    $writer->endElement();
    $writer->startElement('note');
    $writer->writeAttribute('type', 'archival status');
    $writer->text($this->mediaInfo['archival_status']);
    $writer->endElement();
    if ($this->mediaInfo['tape_label']) {
      $writer->startElement('note');
      $writer->writeAttribute('type', 'tape label');
      $writer->text($this->mediaInfo['tape_label']);
      $writer->endElement();
    }
    if ($this->mediaInfo['tape_status']) {
      $writer->startElement('note');
      $writer->writeAttribute('type', 'tape status');
      $writer->text($this->mediaInfo['tape_status']);
      $writer->endElement();
    }
  }

  /**
   * Add relationships to the current object.
   *
   * @see IslandoraBatchObject::addRelationships
   */
  public function addRelationships() {
    module_load_include('inc', 'utailla', 'includes/migration.db');
    $resources = utailla_get_media_file_resources($this->mediaInfo['media_file_id']);
    $found_collection = FALSE;
    foreach ($resources as $collection) {
      // False collections are in the DB but not Fedora.
      if ($collection) {
        $this->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $collection);
        $found_collection = TRUE;
      }
    }
    return $found_collection;
  }
}
