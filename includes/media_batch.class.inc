<?php
/**
 * @file
 * Media file migration batch class file.
 */

class UtaillaMediaBatch extends IslandoraBatchPreprocessor {
  protected $dontMap = array(13, 3, 15, 20, 21, 22, 26);

  /**
   * Queries the migration DB for objects to make.
   *
   * @see IslandoraBatchPreprocessor::preprocess()
   */
  public function preprocess() {
    module_load_include('inc', 'utailla', 'includes/migration.db');
    module_load_include('inc', 'utailla', 'includes/utilities');

    // So much files. Need many memory... ALL YOUR MEMORY ARE BELONG TO PHP.
    ini_set('memory_limit', -1);

    $genres = utailla_get_genres();
    $sanitized_genres = array();
    foreach ($genres as $key => $genre) {
      $sanitized_genres[$key] = utailla_get_decoded_row_recursive($genre);
    }
    $mediums = utailla_get_mediums();
    foreach ($mediums as $key => $medium) {
      $sanitized_mediums[$key] = utailla_get_decoded_row_recursive($medium);
    }
    $encodings = utailla_get_encodings();
    $platforms = utailla_get_platforms();

    $media_files = utailla_get_media_files();
    $objects = array();
    foreach ($media_files as $media_file) {
      if (!in_array($media_file->media_format_id, $this->dontMap)) {
        $object = new UtaillaMediaBatchObject(
          $this->connection,
          utailla_get_decoded_row_recursive((array) $media_file),
          $sanitized_genres,
          $encodings,
          $platforms,
          $sanitized_mediums,
          $this->parameters
        );
        $this->addToDatabase($object, $object->getResources());
        $objects[] = $object;
      }
    }
    return $objects;
  }
}
