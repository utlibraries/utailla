<?php
/**
 * @file
 * Helper functions for dev/test.
 */

/**
 * Prep for deving/testing the media migration.
 */
function utailla_dev_prep($target_dir) {
  module_load_include('inc', 'utailla', 'includes/migration.db');
  $steps = 5;
  $records = array();
  $scp_conn = ssh2_connect('islandora-ailla.lib.utexas.edu');
  ssh2_auth_password($scp_conn, drush_get_option('scp_user'), drush_get_option('password'));
  // Fix broken DB.
  $db = utailla_get_migration_database();
  // Get all types.
  $type_results = utailla_get_migration_database()
    ->select('media_format')
    ->fields('media_format', array('media_format_id'))
    ->execute()
    ->fetchAllAssoc('media_format_id');
  $types = array_keys($type_results);
  $counted_types = array();
  // Count each type.
  foreach ($types as $type) {
    $count = utailla_get_migration_database()
      ->select('media_file')
      ->condition('media_format_id', $type)
      ->countQuery()
      ->execute()
      ->fetchField();
    // Describe sample.
    if ($count) {
      $step_size = floor($count / ($steps - 1) - 1);
      for ($step = 0; $step < $steps; $step++) {
        $offset = $step * $step_size;
        $record = utailla_get_migration_database()
          ->select('media_file', 'm')
          ->fields('m')
          ->condition('media_format_id', $type)
          ->range($offset, 1)
          ->execute()
          ->fetchAssoc('media_file_id');
        $records[] = $record;
        // SCP some files down.
        if (($step % 2) == 1) {
          mkdir("$target_dir{$record['directory_path']}", 0777, TRUE);
          ssh2_scp_recv(
            $scp_conn,
            "/ailla/media{$record['directory_path']}{$record['identifier']}",
            "$target_dir{$record['directory_path']}{$record['identifier']}"
          );
        }
      }
    }
  }
  // Backup and subset DB.
  $db->query('CREATE TABLE media_file_backup SELECT * FROM media_file');
  $db->query('truncate media_file');
  db_set_active('for_migration');
  $nullable = array('orig_rec_speed');
  $unsetable = array('archival_object');
  foreach ($records as $record) {
    foreach ($record as $field_name => &$field) {
      if ($field === '' && in_array($field_name, $nullable)) {
        $field = NULL;
      }
      elseif ($field === '' && in_array($field_name, $unsetable)) {
        unset($record[$field_name]);
      }
    }
    db_insert('media_file')
      ->fields($record)
      ->execute();
  }
  db_set_active();
}
