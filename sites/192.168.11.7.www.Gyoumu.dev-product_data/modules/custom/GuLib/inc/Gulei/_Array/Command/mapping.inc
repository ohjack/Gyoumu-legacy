<?php
namespace Gulei\_Array\Command;

class mapping extends \Gulei\_Array\Command\_Abstract
{
  public function execute($args = null)
  {
    list($mapping) = $args;

    $Mapper = new \Gulei\Mapper\Basic($this->ArryObj);
    return $Mapper->exe($mapping);
  }
}