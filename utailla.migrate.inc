<?php
/**
 * @file
 * Migrate hooks.
 */

/**
 * Implements hook_migrate_api().
 */
function utailla_migrate_api() {
  return array(
    'api' => 2,
    'groups' => array(
      'utailla' => array(
        'title' => t('UTAilla Migrations'),
      ),
    ),
    'migrations' => array(
      UTAILLA_USER_MIGRATION => array(
        'class_name' => 'UtaillaUserMigration',
        'group_name' => 'utailla',
      ),
    ),
    'migrations' => array(
      UTAILLA_CONTACT_MIGRATION => array(
        'class_name' => 'UtaillaContactMigration',
        'group_name' => 'utailla',
      ),
    ),
  );
}
