<?php
/**
 * @file
 * Main user migration class.
 */

/**
 * Main user migration class.
 */
class UtaillaUserMigration extends UtaillaBaseMigration {

  /**
   * Constructor.
   *
   * @see UtaillaBaseMigration::__construct()
   */
  public function __construct($arguments) {
    module_load_include('inc', 'utailla', 'includes/migration.db');
    parent::__construct($arguments);
    $this->roles = array(
      'S' => 'superuser',
      'A' => 'admin',
      'D' => 'depositor',
      'N' => 'authenticated user',
    );
    $query = utailla_get_migration_database()
      ->select('user');
    $query->fields(
      'user',
      array(
        'user_id',
        'user_name',
        'email',
        'date_created',
        'date_last_used',
        'admin',
        'admin_initials',
        'acc_license',
        'first_name',
        'last_name',
        'affiliation',
        'country',
      )
    );
    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    $this->map = utailla_get_user_migration_map();
    $this->addFieldMapping('name', 'user_name');
    $this->addFieldMapping('mail', 'email');
    $this->addFieldMapping('created', 'date_created');
    $this->addFieldMapping('access', 'date_last_used');
    $this->addFieldMapping('login', 'date_last_used');
    // Custom fields.
    $this->addFieldMapping('alternate_email', 'email1');
    $this->addFieldMapping('admin_initials', 'admin_initials')->callbacks(array($this, 'decode'));
    $this->addFieldMapping('first_name', 'first_name')->callbacks(array($this, 'decode'));
    $this->addFieldMapping('last_name', 'last_name')->callbacks(array($this, 'decode'));
    $this->addFieldMapping('affiliation', 'affiliation')->callbacks(array($this, 'decode'));
    $this->addFieldMapping('acc_license', 'acc_license')->callbacks(array($this, 'mapLicense'));
    $this->addFieldMapping('role_names', 'admin')->callbacks(array($this, 'mapRole'));
    $this->addFieldMapping('country', 'country');
    $this->addFieldMapping('status')->defaultValue(1);
  }

  /**
   * Override so we can get data from more tables.
   *
   * @see Migration::prepareRow()
   */
  public function prepareRow($row) {
    // Expand on original query.
    $query = Database::getConnection('default', 'for_migration')
      ->select('user');
    $query->condition('user.user_id', $row->user_id);
    $query->leftJoin('contributor', NULL, 'user.user_id = contributor.user_id');
    $query->leftJoin(
      'contact_info',
      NULL,
      'contact_info.contact_id = contributor.contact_id'
    );
    $query->fields(
      'contact_info',
      array(
        'email1',
        'email2',
        'homephone',
        'cellphone',
        'workphone',
        'fax',
        'address1',
        'address2',
        'website1',
        'website2',
      )
    );
    $result = $query->execute()->fetchAssoc('user_id');
    foreach ($result as $column => $value) {
      $row->$column = $value;
    }
    $this->rollbackAction = $this->defaultRollbackAction;
    return TRUE;
  }

  /**
   * During migration transforms the license for Drupal.
   */
  protected function mapLicense($old_value) {
    if ($old_value == 'Y') {
      return 1;
    }
    else {
      return 0;
    }
  }

  /**
   * During migration transforms the role for Drupal.
   */
  protected function mapRole($old_value) {
    return $this->roles[$old_value];
  }
}
