<?php
namespace Gulei\Util;

class BinTest
{
  private $or_trace = array();
  private $or = false;

  private $and_trace = array();
  private $and = true;

  public function testAnd($val){
    $this->and_trace[] = $val;
    $this->and = $this->and && $val;
  }

  public function testOr($val){
    $this->or_trace = $val;
    $this->or =  $this->or || $val;
  }

  public function getAndVal(){
    return $this->and;
  }

  public function getOrVal(){
    return $this->or;
  }
}