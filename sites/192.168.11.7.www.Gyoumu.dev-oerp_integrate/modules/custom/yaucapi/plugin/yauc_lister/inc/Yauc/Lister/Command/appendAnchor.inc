<?php
namespace Yauc\Lister\Command;

class appendAnchor extends \Gulei\Command\WithReference\_Abstract
{
  public function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');
    return $rec['anchor'] . $rec[$name];
  }
}