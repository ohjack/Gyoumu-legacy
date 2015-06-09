<?php
namespace Gulei\Reference\Wrapper;

abstract class _Abstract
{
  protected $ref;

  function __construct($newRef = array())
  {
    $this->ref = new \Gulei\Reference\Basic();
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