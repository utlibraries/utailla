<?php

/**
 * @file
 * Some helper iterator classes.
 */


class UtaillaFileTraversalIterator extends FilterIterator implements Serializable, UtaillaIteratorIterator {
  protected $iterator;
  public function __construct($id) {
    $this->iterator = new UtaillaAppendIterator();
    $this->iterator->append(new UtaillaRecursiveIteratorIterator(new UtaillaTraversalIterator($id)));
    parent::__construct($this->iterator);
  }

  public function accept() {
    return !array_intersect($this->current()->models, array('islandora:collectionCModel', UTAILLA_RESOURCE_CMODEL));
  }

  public function getIterators() {
    $iterators = array();

    foreach ($this->iterator->getIterators() as $iterator) {
      if ($iterator instanceof UtaillaIteratorIterator) {
        $iterators = array_merge($iterators, $iterator->getIterators());
      }
      else {
        throw new Exception('Bad juju.');
      }
    }

    return $iterators;
  }

  public function serialize() {
    return serialize($this->getIterators());
  }
  public function unserialize($data) {
    $this->iterator = new UtaillaAppendIterator();
    foreach (unserialize($data) as $iterator) {
      if ($iterator instanceof RecursiveIterator) {
        $iterator = new UtaillaRecursiveIteratorIterator($iterator);
      }
      $this->iterator->append($iterator);
    }
    parent::__construct($this->iterator);
    //dsm($this->iterator->valid() ? 'valid' : 'invalid', 'inner file');
  }
  public function valid() {
    $valid = parent::valid();
    //dsm($valid ? 'valid' : 'invalid', __CLASS__);
    return $valid;
  }
}

interface UtaillaIteratorIterator {
  /**
   * Get all the contained iterators, in an array.
   *
   * @return array
   *   The array of iterators in the order they should be used.
   */
  public function getIterators();
}

class UtaillaAppendIterator extends AppendIterator implements Serializable, UtaillaIteratorIterator {
  //public function hasChildren() {
  //  return $this->getInnerIterator()->hasChildren();
  //}
  //public function getChildren() {
  //  return $this->getInnerIterator()->getChildren();
  //}

  public function getIterators() {
    return iterator_to_array($this->getArrayIterator());
  }

  public function serialize() {
    throw new Exception('Is not possible!');
    //$iterators = $this->getArrayIterator();
    //return serialize(array_filter(iterator_to_array($iterators), function ($iterator) {
    //  return $iterator->valid();
    //}));
  }
  public function unserialize($data) {
    //parent::__construct();
    //foreach (unserialize($data) as $iterator) {
    //  $this->append($iterator);
    //}
  }
  public function valid() {
    $valid = parent::valid();
    //dsm($valid ? 'valid' : 'invalid', __CLASS__);
    return $valid;
  }
}

class UtaillaRecursiveIteratorIterator extends RecursiveIteratorIterator implements Serializable, UtaillaIteratorIterator  {
  public function getIterators() {
    $iterators = array();
    $current_depth = $this->getDepth();
    foreach (range($current_depth, 0) as $depth) {
      $iterator = $this->getSubIterator($depth);
      if ($depth < $current_depth) {
        $iterator->next();
      }
      $iterators[] = $iterator;
    }
    //dsm(serialize($iterators), 'qwerqwer');
    return $iterators;
  }
  public function serialize() {
    throw new Exception('Is not possible!');
    //$iterators = $this->getIterators();
    //return serialize($iterators);
  }

  public function unserialize($data) {
    //$iterators = unserialize($data);
    //$asdf = new UtaillaAppendRecursiveIteratorIterator();
    //foreach ($iterators as $iterator) {
    //  $asdf->append($iterator);
    //}
    //parent::__construct($asdf);
  }
  public function valid() {
    $valid = parent::valid();
    //dsm($valid ? 'valid' : 'invalid', __CLASS__);
    return $valid;
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
    $this->value = isset($value['object']['value']) ?
      $value['object']['value'] :
      FALSE;
  }

  protected $rewound = FALSE;
  public function rewind() {
    if (!$this->rewound) {
      $this->page = -1;
      $this->buffer = array();
      $this->next();
      $this->rewound = TRUE;
    }
  }

  public function valid() {
    $valid =  $this->value || (!empty($this->buffer) && $this->total < $this->page * static::SLICE_SIZE);
    //dsm("{$this->id}: " . ($valid ? 'valid' : 'invalid'), __CLASS__);
    return $valid;
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
