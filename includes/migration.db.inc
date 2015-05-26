<?php
/**
 * @file
 * Migration database related functions.
 */

/**
 * Get a migration database.
 */
function utailla_get_migration_database() {
  return Database::getConnection('default', 'for_migration');
}

/**
 * Gets all the info relevant to collections.
 */
function utailla_get_collection_info() {
  $query = utailla_get_migration_database()
    ->select('collection', 'c')
    ->fields('c');
  $collections = $query->execute()->fetchAllAssoc('collection_id');
  foreach ($collections as $collection_id => &$collection) {
    $collection->descriptions = utailla_get_migration_database()
      ->select('collection_description', 'cd')
      ->fields('cd')
      ->condition('cd.collection_id', $collection_id)
      ->execute()
      ->fetchAll();

    $collection->titles = utailla_get_migration_database()
      ->select('collection_title', 'ct')
      ->fields('ct')
      ->condition('ct.collection_id', $collection_id)
      ->execute()
      ->fetchAll();

    $collection->countries = utailla_get_migration_database()
      ->select('collection_to_country', 'cc')
      ->fields('cc')
      ->condition('cc.collection_id', $collection_id)
      ->execute()
      ->fetchAll();

    $collection->languages = utailla_get_migration_database()
      ->select('language_to_collection', 'lc')
      ->fields('lc')
      ->condition('lc.collection_id', $collection_id)
      ->execute()
      ->fetchAll();
  };
  return $collections;
}

/**
 * PID of a collection if it has been migrated.
 */
function utailla_migrated_collection_pid($collection_id) {
  return db_select('utailla_collection_map')
    ->fields('utailla_collection_map', array('pid'))
    ->condition('utailla_collection_map.collection_id', $collection_id)
    ->execute()
    ->fetchField();
}

/**
 * PID of a language if it has been migrated.
 */
function utailla_migrated_language_pid($language_id) {
  return db_select('utailla_language_map')
    ->fields('utailla_language_map', array('pid'))
    ->condition('utailla_language_map.language_id', $language_id)
    ->execute()
    ->fetchField();
}

/**
 * PID of a country if it has been migrated.
 */
function utailla_migrated_country_pid($country_id) {
  return db_select('utailla_country_map')
    ->fields('utailla_country_map', array('pid'))
    ->condition('utailla_country_map.country_id', $country_id)
    ->execute()
    ->fetchField();
}

/**
 * Log a collection mapping from the old DB to Fedora.
 */
function utailla_log_collection_mapping($collection_id, $pid) {
  db_insert('utailla_collection_map')
    ->fields(
      array(
        'collection_id' => $collection_id,
        'pid' => $pid,
      )
    )
    ->execute();
}
