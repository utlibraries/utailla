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

/**
 * Get the basic info relevant to media_files.
 *
 * @note
 *   AILLA wishes to constrain media files that belong to resource only.
 */
function utailla_get_media_files() {
  $db = utailla_get_migration_database();
  $query = $db->select('media_file', 'media');
  $query->rightJoin('resource_to_media', 'resource', 'resource.media_file_id = media.media_file_id');
  return $query->fields('media', array())
    ->execute()
    ->fetchAllAssoc('media_file_id');
}

/**
 * Get all genres.
 */
function utailla_get_genres() {
  return utailla_get_migration_database()
    ->select('media_content_type')
    ->fields('media_content_type')
    ->execute()
    ->fetchAllAssoc('media_content_id');
}

/**
 * Get all encodings.
 */
function utailla_get_encodings() {
  return utailla_get_migration_database()
    ->select('encoding')
    ->fields('encoding')
    ->execute()
    ->fetchAllAssoc('encoding_id');
}

/**
 * Get all platforms.
 */
function utailla_get_platforms() {
  return utailla_get_migration_database()
    ->select('digi_platform')
    ->fields('digi_platform')
    ->execute()
    ->fetchAllAssoc('digi_platform_id');
}

/**
 * Get all mediums.
 */
function utailla_get_mediums() {
  return utailla_get_migration_database()
    ->select('orig_medium')
    ->fields('orig_medium')
    ->execute()
    ->fetchAllAssoc('orig_medium_id');
}

/**
 * Get all the resources for the media_file.
 */
function utailla_get_media_file_resources($id) {
  $resources = utailla_get_migration_database()
    ->select('resource_to_media')
    ->fields('resource_to_media', array('resource_id'))
    ->condition('resource_to_media.media_file_id', $id)
    ->execute()
    ->fetchAllAssoc('resource_id');
  foreach ($resources as $resource_id => &$resource) {
    $resource = utailla_migrated_resource_pid($resource_id);
  }
  return array_values($resources);
}

/**
 * Get all the languages for the media_file.
 */
function utailla_get_media_file_languages($id) {
  $languages = utailla_get_migration_database()
    ->select('language_to_media')
    ->fields('language_to_media', array('language_id'))
    ->condition('language_to_media.media_file_id', $id)
    ->execute()
    ->fetchAllAssoc('language_id');
  foreach ($languages as $language_id => &$language) {
    $language = utailla_migrated_language_pid($language_id);
  }
  return array_values($languages);
}

/**
 * PID of a media file if it has been migrated.
 */
function utailla_migrated_media_file_pid($id) {
  return db_select('utailla_media_file_map')
    ->fields('utailla_media_file_map', array('pid'))
    ->condition('utailla_media_file_map.media_file_id', $id)
    ->execute()
    ->fetchField();
}

/**
 * Log a media file mapping from the old DB to Fedora.
 */
function utailla_log_media_file_mapping($id, $pid) {
  db_insert('utailla_media_file_map')
    ->fields(
      array(
        'media_file_id' => $id,
        'pid' => $pid,
      )
    )
    ->execute();
}
