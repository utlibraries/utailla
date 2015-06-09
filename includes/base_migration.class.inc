<?php
/**
 * @file
 * Base migration class.
 */

class UtaillaBaseMigration extends Migration {

  /**
   * During migration decode HTML entities.
   */
  protected function decode($old_value) {
    module_load_include('inc', 'utailla', '/includes/utilities');
    $value_array = utailla_get_decoded_row_recursive($old_value);
    return $value_array[0];
  }

  /**
   * Constructor.
   *
   * @see Migration::__construct()
   */
  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->destination = new MigrateDestinationUser(array());

    $this->addFieldMapping('second_alternate_email', 'email2');
    $this->addFieldMapping('home_phone', 'homephone');
    $this->addFieldMapping('cell_phone', 'cellphone');
    $this->addFieldMapping('work_phone', 'workphone');
    $this->addFieldMapping('fax', 'fax');
    $this->addFieldMapping('address', 'address1')->callbacks(array($this, 'decode'));
    $this->addFieldMapping('alt_address', 'address2')->callbacks(array($this, 'decode'));
    $this->addFieldMapping('website', 'website1');
    $this->addFieldMapping('alt_website', 'website2');
  }
}
