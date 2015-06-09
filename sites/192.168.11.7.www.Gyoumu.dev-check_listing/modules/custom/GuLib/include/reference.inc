<?php
namespace Gulei;

class Reference
{
  private $ref = array();

  public function add(array $newRefs)
  {
    $this->ref = array_merge_recursive_simple($this->ref, $newRefs);
  }

  public function val()
  {
    $val = $this->ref;
    foreach(func_get_args() as $anchor){
      if(isset($val[$anchor])){
        $val = $val[$anchor];
      }
      else{
        return null;
      }
    }
    return $val;
  }

  public function clear()
  {
    $this->ref = array();
  }
}

abstract class AbstractReferenceWrapper
{
  protected $ref;

  function __construct($newRef = array())
  {
    $this->ref = new Reference();
    $this->ref->add($newRef);
  }

  public function addReference($newRef)
  {
    if(is_array($newRef)){
      $this->ref->add($newRef);
    }
    return $this;
  }

  public function clearReference()
  {
    $this->ref->clear();
    return $this;
  }
}