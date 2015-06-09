<?php
namespace Oerp\Command;

class isCarrierUps extends \Gulei\Command\WithReference\WithTable
{
  public function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');
    return strpos($rec[$name], 'UPS') === 0;
  }
}