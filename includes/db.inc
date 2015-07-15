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
