<?php
namespace Gulei\Util;

class BinTest
{
  private $or = false;
  private $and = true;

  public function testAnd($val){
    $this->and = $this->and && $val;
  }

  public function testOr($val){
    $this->or =  $this->or || $val;
  }

  public function getAndVal(){
    return $this->and;
  }

  public function getOrVal(){
    return $this->or;
  }
}