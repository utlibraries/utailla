<?php
/**
 * @file
 * Contact migration class file.
 */

class UtaillaContactMigration extends UtaillaBaseMigration {

  /**
   * Constructor.
   *
   * @see UtaillaBaseMigration::__construct()
   */
  public function __construct($arguments) {
    module_load_include('inc', 'utailla', 'includes/migration.db');
    parent::__construct($arguments);
    $query = utailla_get_migration_database()
      ->select('contact_info', 'c');
    $query->leftJoin('organization', 'org', 'org.contact_id = c.contact_id');
    $query->leftJoin('contributor', 'con', 'c.contact_id = con.contact_id');
    $query->leftJoin('user', NULL, 'user.user_id = con.user_id');
    $query->isNull('user.user_id');
    $query->fields('org', array('fullname'))
      ->fields('con', array('first_name', 'last_name'))
      ->fields('c');
    $this->source = new MigrateSourceSQL($query);
    $this->map = utailla_get_contact_migration_map();

    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('mail', 'email1');
  }

  /**
   * Override to conditionally set name.
   *
   * @see Migration::prepareRow()
   */
  public function prepareRow($row) {
    module_load_include('inc', 'utailla', 'includes/utilities');
    if ($row->fullname) {
      $row->name = $row->fullname;
    }
    elseif ($row->first_name || $row->last_name) {
      $row->name = utailla_concat_name_parts($row);
    }
    else {
      $row->name = $row->contact_id;
    }
    $row->name = truncate_utf8($row->name, 60, TRUE);
  }
}
