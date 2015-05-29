<?php
/**
 * @file
 * ailla_platform_feature.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function ailla_platform_feature_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'view fedora repository objects'.
  $permissions['view fedora repository objects'] = array(
    'name' => 'view fedora repository objects',
    'roles' => array(
      'administrator' => 'administrator',
    ),
    'module' => 'islandora',
  );

  return $permissions;
}
