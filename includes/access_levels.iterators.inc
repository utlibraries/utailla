<?php

/**
 * @file
 * Some helper iterator classes.
 */

/**
 * Iterface to help with serialization.
 */
interface UtaillaIteratorIteratorInterface {
  /**
   * Get all the contained iterators, in an array.
   *
   * @return array
   *   The array of serializable iterators in the order they should be used.
   */
  public function getIterators();
}

/**
 * A serializable FilterIterator.
 *
 * Accepts objects which have neither the 'islandora:collectionCModel' nor the
 * UTAILLA_RESOURCE_CMODEL.
 */
class UtaillaFileTraversalIterator extends FilterIterator implements Serializable, UtaillaIteratorIteratorInterface {
  /**
   * @var UtaillaAppendIterator
   *   The iterator from which to pull filtered results.
   */
  protected $iterator;
  protected $excludedModels;

  /**
   * Constructor.
   *
   * @param string $id
   *   The ID over which to iterate.
   */
  public function __construct($id, $exclude_models) {
    $this->excludedModels = $exclude_models;
    $this->iterator = new UtaillaAppendIterator();
    $this->iterator->append(new UtaillaRecursiveIteratorIterator(new UtaillaTraversalIterator($id)));
    parent::__construct($this->iterator);
  }

  /**
   * Implementing interface.
   *
   * @see FilterIterator::accept()
   */
  public function accept() {
    return !array_intersect($this->current()->models, $this->excludedModels);
  }

  /**
   * Implementing interface.
   *
   * @see UtaillaIteratorIteratorInterface::getIterators()
   */
  public function getIterators() {
    $iterators = array();

    foreach ($this->iterator->getIterators() as $iterator) {
      if ($iterator instanceof UtaillaIteratorIteratorInterface) {
        $iterators = array_merge($iterators, $iterator->getIterators());
      }
      else {
        throw new Exception('Bad juju.');
      }
    }

    return $iterators;
  }

  /**
   * Implementing interface.
   *
   * @see Serializable::serialize()
   */
  public function serialize() {
    return serialize(array(
      $this->getIterators(),
      $this->excludedModels,
    ));
  }

  /**
   * Implementing interface.
   *
   * @see Serializable::unserialize()
   */
  public function unserialize($data) {
    list($iterators, $this->excludedModels) = unserialize($data);
    $this->iterator = new UtaillaAppendIterator();
    foreach ($iterators as $iterator) {
      if ($iterator instanceof RecursiveIterator) {
        $iterator = new UtaillaRecursiveIteratorIterator($iterator);
      }
      $this->iterator->append($iterator);
    }

    // XXX: Must explicitly call with "parent::", due to SPL implementation.
    // Will complain about not being initialized correctly if one tries to use
    // late static binding.
    parent::__construct($this->iterator);
  }
}

class UtaillaAppendIterator extends AppendIterator implements Serializable, UtaillaIteratorIteratorInterface {
  /**
   * Implementing interface.
   *
   * @see UtaillaIteratorIteratorInterface::getIterators()
   */
  public function getIterators() {
    return iterator_to_array($this->getArrayIterator());
  }

  /**
   * Implementing interface.
   *
   * @see Serializable::serialize()
   */
  public function serialize() {
    throw new Exception('Is not possible!');
  }

  /**
   * Implementing interface.
   *
   * @see Serializable::unserialize()
   */
  public function unserialize($data) {
    // No-op. Not serializable.
  }
}

/**
 * A RecursiveIteratorIterator from which we can easily grab the iterators.
 */
class UtaillaRecursiveIteratorIterator extends RecursiveIteratorIterator implements Serializable, UtaillaIteratorIteratorInterface  {
  /**
   * Implementing interface.
   *
   * @see UtaillaIteratorIteratorInterface::getIterators()
   */
  public function getIterators() {
    $iterators = array();

    $current_depth = $this->getDepth();
    foreach (range($current_depth, 0) as $depth) {
      $iterator = $this->getSubIterator($depth);
      if ($depth < $current_depth) {
        // Iterators "deeper" on the stack need to be moved to the next
        // element, so they avoid "re-recursing" when we later give them to a
        // RecursiveIteratorIterator.
        $iterator->next();
      }
      $iterators[] = $iterator;
    }

    return $iterators;
  }

  /**
   * Implementing interface.
   *
   * @see Serializable::serialize()
   */
  public function serialize() {
    throw new Exception('Is not possible!');
  }

  /**
   * Implementing interface.
   *
   * @see Serializable::unserialize()
   */
  public function unserialize($data) {
    // No-op. Not serializable.
  }
}

/**
 * A RecursiveIterator to iterate over Islandora's collection structure.
 */
class UtaillaTraversalIterator implements RecursiveIterator {
  /**
   * @var string $id
   *   The ID of the object being iterated.
   */
  protected $id;

  /**
   * @var array $buffer
   *   An array of associative arrays representing a subset of the results
   *   being iterated. Inner arrays are structured as per
   *   islandora_basic_collection_get_member_objects().
   */
  protected $buffer = array();

  /**
   * @var int $page
   *   The "page" of results, in the iteration.
   */
  protected $page;

  /**
   * @var int $total
   *   The total number of results over which we should iterate.
   */
  protected $total;

  /**
   * @var bool $rewound
   *   A boolean flag indicating if this iterator has been rewound.
   */
  protected $rewound = FALSE;

  /**
   * @var int
   *   The number of objects "per page", while iterating.
   */
  const SLICE_SIZE = 20;

  /**
   * Constructor.
   *
   * @param string $id
   *   The PID under which we will iterate.
   */
  public function __construct($id) {
    $this->id = $id;
  }

  /**
   * Implementing interface.
   *
   * @see RecursiveIterator::current()
   */
  public function current() {
    return islandora_object_load($this->value);
  }

  /**
   * Implementing interface.
   *
   * @see RecursiveIterator::key()
   */
  public function key() {
    return $this->current()->id;
  }

  /**
   * Implementing interface.
   *
   * @see RecursiveIterator::next()
   */
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

  /**
   * Implementing interface.
   *
   * @see RecursiveIterator::rewind()
   */
  public function rewind() {
    // Prevent rewinding, other than the initial.
    if (!$this->rewound) {
      // The "next()" call will increment this to page 0.
      $this->page = -1;
      $this->buffer = array();
      $this->next();
      $this->rewound = TRUE;
    }
  }

  /**
   * Implementing interface.
   *
   * @see RecursiveIterator::valid()
   */
  public function valid() {
    // We have a value, or the hope of getting more.
    return $this->value || (!empty($this->buffer) && $this->total < $this->page * static::SLICE_SIZE);
  }

  /**
   * Populate our buffer for iteration.
   */
  protected function query() {
    list($this->total, $this->buffer) = islandora_basic_collection_get_member_objects(islandora_object_load($this->id), $this->page, static::SLICE_SIZE);
  }

  /**
   * Implementing interface.
   *
   * @see RecursiveIterator::hasChildren()
   */
  public function hasChildren() {
    return $this->current() && array_intersect($this->current()->models, array('islandora:collectionCModel', UTAILLA_RESOURCE_CMODEL));
  }

  /**
   * Implementing interface.
   *
   * @see RecursiveIterator::getChildren()
   */
  public function getChildren() {
    return new UtaillaTraversalIterator($this->current()->id);
  }
}

/**
 * Helper class; work around PHP bug.
 *
 * The ArrayIterator class claims to be serializable; however, it does not
 * maintain the offset within the array, nor does it offer a way to discover it.
 *
 * @see http://stackoverflow.com/a/20069139
 */
class UtaillaArrayIterator extends ArrayIterator {
  private $position = 0;

  /**
   * Proceed to the next element, maintaining our position.
   */
  public function next() {
    $this->position++;
    parent::next();
  }

  /**
   * Rewind the iterator, resetting our position.
   */
  public function rewind() {
    $this->position = 0;
    parent::rewind();
  }

  /**
   * Seek the iterator, updating our position.
   */
  public function seek($position) {
    $this->position = $position;
    parent::seek($position);
  }

  /**
   * Reset the position when unserializing.
   */
  public function unserialize($serialized) {
    parent::unserialize($serialized);

    // PHP does not maintain the position across (de)serialization... Let's set
    // it up again...
    $this->seek($this->position);
  }
}
