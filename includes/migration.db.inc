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
 * Get all the info relevant to contributors.
 */
function utailla_get_contributor_info($offset = NULL, $limit = 100) {
  $query = utailla_get_migration_database()
    ->select('contributor')
    ->range($offset, $limit)
    ->fields('contributor');
  $contributors = $query->execute()->fetchAllAssoc('contributor_id');
  foreach ($contributors as $id => &$contributor) {
    $contributor->languages = utailla_get_migration_database()
      ->select('contributor_to_language', 'cl')
      ->fields('cl')
      ->condition('contributor_id', $id)
      ->execute()
      ->fetchAll();
    $contributor->organizations = utailla_get_migration_database()
      ->select('contributor_to_organization', 'co')
      ->fields('co')
      ->condition('contributor_id', $id)
      ->execute()
      ->fetchAll();
  }
  return $contributors;
}

/**
 * Gets all the info relevant to organizations.
 */
function utailla_get_organization_info() {
  $query = utailla_get_migration_database()
    ->select('organization', 'o')
    ->fields('o');
  $organizations = $query->execute()->fetchAllAssoc('organization_id');

  foreach ($organizations as $id => &$org) {
    $org->languages = utailla_get_migration_database()
      ->select('organization_to_language', 'ol')
      ->fields('ol')
      ->condition('organization_id', $id)
      ->execute()
      ->fetchAll();
  }

  return $organizations;
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

/**
 * PID of an organization if it has been migrated.
 */
function utailla_migrated_organization_pid($organization_id) {
  return db_select('utailla_organization_map')
    ->fields('utailla_organization_map', array('pid'))
    ->condition('utailla_organization_map.organization_id', $organization_id)
    ->execute()
    ->fetchField();
}

/**
 * Log an organization mapping from the old DB to Fedora.
 */
function utailla_log_organization_mapping($organization_id, $pid) {
  db_insert('utailla_organization_map')
    ->fields(
      array(
        'organization_id' => $organization_id,
        'pid' => $pid,
      )
    )
    ->execute();
}

/**
 * Log a contributor mapping.
 */
function utailla_log_contributor_mapping($id, $pid) {
  db_insert('utailla_contributor_map')
    ->fields(
      array(
        'contributor_id' => $id,
        'pid' => $pid,
      )
    )
    ->execute();
}

/**
 * PID of a contributor that has been migrated.
 */
function utailla_migrated_contributor_pid($id) {
  return db_select('utailla_contributor_map')
    ->fields('utailla_contributor_map', array('pid'))
    ->condition('utailla_contributor_map.contributor_id', $id)
    ->execute()
    ->fetchField();
}

/**
 * Get migrated UID if one exists.
 */
function utailla_get_migrated_user_uid($user_id) {
  $map = utailla_get_user_migration_map();
  $source = $map->lookupDestinationID(array($user_id));
  return isset($source['destid1']) ? $source['destid1'] : NULL;
}

/**
 * Get the user migration map object.
 */
function utailla_get_user_migration_map() {
  return new MigrateSQLMap(
    UTAILLA_USER_MIGRATION,
    array(
      'user_id' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
    ),
    MigrateDestinationUser::getKeySchema()
  );
}

/**
 * Get migrated UID if one exists from contact_id.
 */
function utailla_get_migrated_contact_uid($id) {
  $map = utailla_get_contact_migration_map();
  $source = $map->lookupDestinationID(array($id));
  return isset($source['destid1']) ? $source['destid1'] : NULL;
}

/**
 * Get the contact migration map object.
 */
function utailla_get_contact_migration_map() {
  return new MigrateSQLMap(
    UTAILLA_CONTACT_MIGRATION,
    array(
      'contact_id' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'alias' => 'c',
      ),
    ),
    MigrateDestinationUser::getKeySchema()
  );
}

/**
 * Get the number of contributors to migrate.
 */
function utailla_get_contributor_count() {
  $query = utailla_get_migration_database()
    ->select('contributor')
    ->fields('contributor');
  return $query->countQuery()->execute()->fetchField();
}
