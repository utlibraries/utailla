<?php
/**
 * @file
 * Batch related functions.
 */

/**
 * Batch array for migrating contributors.
 */
function utailla_migrate_contributors_batch() {
  return array(
    'operations' => array(array('utailla_migrate_contributors_batch_operation', array())),
    'file' => drupal_get_path('module', 'utailla') . '/includes/contributor_migration.inc',
  );
}

/**
 * Batch array for migrating collections.
 */
function utailla_migrate_collections_batch() {
  return array(
    'operations' => array(array('utailla_migrate_collections_batch_operation', array())),
    'file' => drupal_get_path('module', 'utailla') . '/includes/collection_migration.inc',
  );
}

/**
 * Batch array for migrating organizations.
 */
function utailla_migrate_organizations_batch() {
  return array(
    'operations' => array(array('utailla_migrate_organizations_batch_operation', array())),
    'file' => drupal_get_path('module', 'utailla') . '/includes/organization_migration.inc',
  );
}

/**
 * Batch array for migrating languages.
 */
function utailla_migrate_languages_batch($db = 'for_migration') {
  return array(
    'file' => drupal_get_path('module', 'utailla') . '/includes/utailla.drush.inc',
    'operations' => array(
      array(
        'utailla_migrate_language_batch_operation',
        array($db),
      ),
    ),
  );
}

/**
 * Batch array for migrating countries.
 */
function utailla_migrate_countries_batch($db = 'for_migration') {
  return array(
    'file' => drupal_get_path('module', 'utailla') . '/includes/utailla.drush.inc',
    'operations' => array(
      array(
        'utailla_migrate_countries_batch_operation',
        array($db),
      ),
    ),
  );
}
