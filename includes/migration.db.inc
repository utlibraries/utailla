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
 * Get the total number of rows from the resource import table.
 */
function utailla_get_resource_count() {
  $query = utailla_get_migration_database()
    ->select('resource', 'r')
    ->fields('r');
  $count = $query->countQuery()->execute()->fetchField();
  return $count;
}

/**
 * Gets all the info relevant to resources.
 *
 * @param int $offset
 *   The offset when selecting records from the resource table.
 * @param int $limit
 *   The Number of records to return from the resource table query.
 */
function utailla_get_resource_info($offset = NULL, $limit = UTAILLA_LIMIT_FOR_RESOURCE_IMPORT) {
  // Uses a range on the resource table due to a drush processing issue.
  $query = utailla_get_migration_database()
    ->select('resource', 'r')
    ->fields('r')
    ->range($offset, $limit);
  $resources = $query->execute()->fetchAllAssoc('resource_id');

  foreach ($resources as $resrouce_id => &$resource) {
    $resource->titles = utailla_get_migration_database()
      ->select('resource_title', 'rt')
      ->fields('rt')
      ->condition('rt.resource_id', $resrouce_id)
      ->execute()
      ->fetchAll();

    $resource->descriptions = utailla_get_migration_database()
      ->select('resource_description', 'rd')
      ->fields('rd')
      ->condition('rd.resource_id', $resrouce_id)
      ->execute()
      ->fetchAll();

    $resource->languages = utailla_get_migration_database()
      ->select('language_to_resource', 'lr')
      ->fields('lr')
      ->condition('lr.resource_id', $resrouce_id)
      ->execute()
      ->fetchAll();

    $query = utailla_get_migration_database()
      ->select('contributor_to_resource', 'conr');
    $alias = $query->join('role', 'rl', 'rl.role_id=conr.role_id');
    $resource->contributors = $query
      ->fields('conr', array('contributor_id'))
      ->fields($alias, array('role_id', 'role_name', 'papel_nombre'))
      ->condition('conr.resource_id', $resrouce_id)
      ->execute()
      ->fetchAll();

    $resource->collections = utailla_get_migration_database()
      ->select('collection_to_resource', 'collr')
      ->fields('collr')
      ->condition('collr.resource_id', $resrouce_id)
      ->execute()
      ->fetchAll();

    $query = utailla_get_migration_database()
      ->select('genre_to_resource', 'gr');
    $alias = $query->join('genre', 'g', 'g.genre_id=gr.genre_id');
    $resource->genres = $query->fields($alias)
      ->condition('gr.resource_id', $resrouce_id)
      ->execute()
      ->fetchAll();
  }
  return $resources;
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
 * PID of an resource if it has been migrated.
 */
function utailla_migrated_resource_pid($resource_id) {
  return db_select('utailla_resource_map')
    ->fields('utailla_resource_map', array('pid'))
    ->condition('utailla_resource_map.resource_id', $resource_id)
    ->execute()
    ->fetchField();
}

/**
 * Log an organization mapping from the old DB to Fedora.
 */
function utailla_log_resource_mapping($resource_id, $pid) {
  db_insert('utailla_resource_map')
    ->fields(
      array(
        'resource_id' => $resource_id,
        'pid' => $pid,
      )
    )
    ->execute();
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
 * PID of a collection if it has been migrated.
 */
function utailla_migrated_contributor_pid($collection_id) {
  // TODO: Remove PLACEHOLDER.
  return NULL;
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
