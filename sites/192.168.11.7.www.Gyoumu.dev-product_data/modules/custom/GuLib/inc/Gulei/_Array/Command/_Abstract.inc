<?php
namespace Gulei\_Array\Command;

abstract class _Abstract implements \Gulei\Command\_Interface
{
  protected $ArryObj;

  public function __construct($ArryObj)
  {
    $this->ArryObj = $ArryObj;
  }
}