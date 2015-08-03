<?php
/**
 * @file
 * Database helpers.
 */

/**
 * Upsert a file restriction.
 */
function utailla_upsert_restriction($restriction_info) {
  db_merge('utailla_media_file_restrictions')
    ->key(array('pid' => $restriction_info['pid']))
    ->fields($restriction_info)
    ->execute();
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

/**
 * Set current user's clearance for a PID.
 *
 * @global $user
 */
function utailla_enable_user_media_file_authorization($pid) {
  global $user;
  db_merge('utailla_media_file_authorizations')
    ->key(array('pid' => $pid, 'uid' => $user->uid))
    ->fields(
      array('pid' => $pid, 'uid' => $user->uid, 'authorization_time' => time())
    )
    ->execute();
}

/**
 * Get current user's clearance for a PID.
 *
 * @global $user
 */
function utailla_user_media_file_authorization($pid) {
  global $user;
  return db_select('utailla_media_file_authorizations', 'a')
    ->fields('a')
    ->condition('pid', $pid)
    ->condition('uid', $user->uid)
    ->execute()
    ->fetchAssoc();
}

/**
 * Remove an authorization for the current user on a PID combo.
 *
 * @global $user
 */
function utailla_remove_user_media_file_authorization($pid) {
  global $user;
  db_delete('utailla_media_file_authorizations')
    ->condition('uid', $user->uid)
    ->condition('pid', $pid)
    ->execute();
}

/**
 * Remove all out of date authorizations.
 */
function utailla_remove_outdated_media_file_authorizations() {
  db_delete('utailla_media_file_authorizations')
    ->condition('authorization_time', time() - UTAILLA_AUTHORIZATION_DURATION, '<')
    ->execute();
}

/**
 * Updates all out of date restrictions.
 */
function utailla_update_outdated_media_file_restrictions() {
  db_update('utailla_media_file_restrictions')
    ->fields(
      array(
        'level' => UTAILLA_RESTRICTION_LEVEL_1,
        'end_time' => NULL,
        'password' => NULL,
        'hint' => NULL,
      )
    )
    ->condition('level', UTAILLA_RESTRICTION_LEVEL_3)
    ->condition('end_time', time(), '<')
    ->execute();
}
