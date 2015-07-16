<?php
/**
 * @file
 * Database helpers.
 */

/**
 * Upsert a file restriction.
 */
function utailla_upsert_restriction($restriction_info) {
  drupal_write_record('utailla_media_file_restrictions', $restriction_info);
}

/**
 * Retrieve restriction info.
 *
 * @param string $pid
 *   The PID for which to obtain info.
 *
 * @return array|bool
 *   An associative array contain the info keyed per the
 *   "utailla_media_file_restrictions" table, or boolean FALSE if a restriction
 *   record could not be found.
 */
function utailla_get_restriction($pid) {
  return db_select('utailla_media_file_restrictions', 'r')
    ->fields('r')
    ->condition('pid', $pid)
    ->execute()
    ->fetchAssoc();
}
