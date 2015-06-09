<?php
namespace Gulei\File\Finder;

abstract class _Abstract{
  protected $base;

  abstract public function getList();

  function __construct($base){
    $this->base = rtrim($base, '/');
  }

  function getBase(){
    return $this->base;
  }

  function pregSearch($pat, $ref = null){
    $list = $this->getList();

    $found = array();
    foreach($list as $file){
      if(preg_match($pat, $file, $m)){
        if($ref){
          $found[$m[$ref]] = $file;
        }
        else{
          $found[] = $file;
        }
      }
    }

    return $found;
  }
}