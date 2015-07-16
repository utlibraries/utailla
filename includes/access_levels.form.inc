<?php
/**
 * @file
 * Access level form and processing.
 */

/**
 * Naively obtain the first "file".
 *
 * @param AbstractObject $object
 *   The object from which to start querying.
 * @param int $slice_size
 *   The number of objects for which to query at a time.
 *
 * @return AbstractObject|NULL
 *   An AbstractObject representing the first file found; otherwise, NULL if we
 *   could not find one.
 */
function utailla_get_first_file(AbstractObject $object, $slice_size = 10) {
  if (array_intersect($object->models, array('islandora:collectionCModel', UTAILLA_RESOURCE_CMODEL))) {
    $page = 0;
    list($count, $members) = islandora_basic_collection_get_member_objects($object, $page, $slice_size);
    while (count($members) > 0) {
      foreach ($members as $member) {
        $child = islandora_object_load($member['object']['value']);
        if (!$child) {
          continue;
        }

        return utailla_get_first_file($child, $slice_size);
      }

      $page++;
      list($count, $members) = islandora_basic_collection_get_member_objects($object, $page, $slice_size);
    }
  }
  else {
    return $object;
  }
}

/**
 * Form building function; build out the access level form.
 */
function utailla_access_level_form($form, &$form_state, AbstractObject $object_in) {
  form_load_include($form_state, 'inc', 'utailla', 'includes/access_levels.form');
  module_load_include('inc', 'utailla', 'includes/db');
  module_load_include('inc', 'utailla', 'includes/utilities');

  // XXX: Reload the object to attempt to alleviate any form/caching issues.
  $object = islandora_object_load($object_in->id);

  $first_file = utailla_get_first_file($object);
  $info = (array) utailla_get_restriction($first_file->id) + array(
    'hint' => '',
    'level' => 1,
    'reason_option' => 0,
    'custom_reason' => '',
    'end_time' => NULL,
  );
  $justifications = utailla_access_justification_options();

  // @todo: Only for resources.
  if (TRUE || in_array(UTAILLA_RESOURCE_CMODEL, $object->models)) {
    $files = array();
    list($count, $members) = islandora_basic_collection_get_member_objects($object);
    foreach ($members as $member) {
      $files[$member['object']['value']] = array(
        'title' => $member['title']['value'],
        'modified' => $member['date_modified']['value'],
      );
    }
    $form['files'] = array(
      '#type' => 'tableselect',
      '#title' => t('Files'),
      '#header' => array('title' => t('Title'), 'modified' => t('Date Modified')),
      '#options' => $files,
    );
  }

  $form['password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
  );
  $form['hint'] = array(
    '#type' => 'textfield',
    '#title' => t('Password Hint'),
    '#default_value' => $info['hint'],
  );
  $form['level'] = array(
    '#type' => 'radios',
    '#title' => t('Access Level'),
    '#options' => array(
      1 => t('Level 1: Public Access'),
      2 => t('Level 2: Password'),
      3 => t('Level 3: Time limit'),
      4 => t('Level 4: Contact depositor'),
    ),
    '#default_value' => $info['level'],
  );
  $form['reason_option'] = array(
    '#type' => 'select',
    '#title' => t('Justification for restricting access'),
    '#options' => $justifications,
    '#default_value' => $info['reason_option'],
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          '!value' => 1,
        ),
      ),
    ),
  );
  $form['custom_reason'] = array(
    '#type' => 'textfield',
    '#maxlength' => 256,
    '#title' => t('Explanation for Other justification'),
    '#default_value' => $info['custom_reason'],
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          '!value' => 1,
        ),
        ':input[name=reason_option]' => array(
          // XXX: May have to make lookup of "other" option smarter.
          'value' => count($justifications) - 1,
        ),
      ),
    ),
  );
  $form['end_time'] = array(
    '#type' => 'datepicker',
    '#title' => t('Time Limit'),
    '#default_value' => $info['end_time'],
    '#states' => array(
      'visible' => array(
        ':input[name=level]' => array(
          'value' => 3,
        ),
      ),
    ),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Commit changes'),
    ),
  );

  return $form;
}

/**
 * Form validation handler; validate values submitted to the access level form.
 */
function utailla_access_level_form_validate(&$form, &$form_state) {

}

/**
 * Form submission handler; put the submitted values into effect.
 */
function utailla_access_level_form_submit(&$form, &$form_state) {
  dsm($form_state['values'], 'fsv');
}

function utailla_access_level_batch_op($restriction_info, $parent_id, $objects = array(), &$context = NULL) {
  $sandbox =& $context['sandbox'];

  if (!isset($sandbox['iterator'])) {
    $sandbox['count'] = 0;
    if (!empty($objects)) {
      $sandbox['iterator'] = new ArrayIterator($objects);
    }
    else {
      $sandbox['iterator'] = new UtaillaFileTraversalIterator($parent_id);
    }
    $sandbox['iterator']->rewind();
    if (!$sandbox['iterator']->valid()) {
      $context['message'] = t('Nothing to iterate.');
      return;
    }
  }
  elseif (!$sandbox['iterator']->valid()) {
    $context['message'] = t('Done.');
    return;
  }

  $current = $sandbox['iterator']->current();

  dsm($current->id, 'the id');
  module_load_include('inc', 'utailla', 'includes/utilities');
  //utailla_set_restriction($current, $restriction_info);

  $sandbox['iterator']->next();
  $context['finished'] = $sandbox['count'] / ($sandbox['iterator'] instanceof Countable ?
    count($sandbox['iterator']) :
    $sandbox['count'] + 1);
  $sandbox['count']++;
}

class UtaillaFileTraversalIterator extends FilterIterator {
  public function __construct($id) {
    parent::__construct(new RecursiveIteratorIterator(new UtaillaTraversalIterator($id)));
  }

  public function accept() {
    return !array_intersect($this->current()->models, array('islandora:collectionCModel', UTAILLA_RESOURCE_CMODEL));
  }
}

class UtaillaTraversalIterator implements RecursiveIterator {
  protected $id;
  protected $buffer = array();
  protected $page;
  protected $total;
  const SLICE_SIZE = 20;

  public function __construct($id) {
    $this->id = $id;
  }

  public function current() {
    return islandora_object_load($this->value);
  }

  public function key() {
    return $this->current()->id;
  }

  public function next() {
    if (empty($this->buffer)) {
      $this->page++;
      $this->query();
    }
    $value = array_shift($this->buffer);
    $this->value = $value['object']['value'];
  }

  public function rewind() {
    $this->page = -1;
    $this->buffer = array();
    $this->next();
  }

  public function valid() {
    return $this->value || (!empty($this->buffer) && $this->total < $this->page * static::SLICE_SIZE);
  }

  protected function query() {
    list($this->total, $this->buffer) = islandora_basic_collection_get_member_objects(islandora_object_load($this->id), $this->page, static::SLICE_SIZE);
  }

  public function hasChildren() {
    return $this->current() && array_intersect($this->current()->models, array('islandora:collectionCModel', UTAILLA_RESOURCE_CMODEL));
  }

  public function getChildren() {
    return new UtaillaTraversalIterator($this->current()->id);
  }
}
