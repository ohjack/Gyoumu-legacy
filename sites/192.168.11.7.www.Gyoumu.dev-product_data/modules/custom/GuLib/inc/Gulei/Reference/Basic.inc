<?php
namespace Gulei\Reference;

class Basic
{
  private $ref = array();

  public function add(array $newRefs)
  {
    $Arry = new \Gulei\_Array\Basic($this->ref);
    $this->ref = $Arry->exe("mergeRecursively", $newRefs);
//    $this->ref = array_merge_recursive_simple($this->ref, $newRefs);
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

  public function vals()
  {
    $vals = array();
    foreach(func_get_args() as $name){
      $vals[] = $this->val($name);
    }
    return $vals;
  }

  public function clear()
  {
    $this->ref = array();
  }
}