<?php
namespace Oerp\Command;

class isCarrierUps extends \Gulei\Command\WithReference\_Abstract
{
  public function execute($refVar = null)
  {
    list($rec, $name) = array(
      $this->ref->val('rec'),
      $this->ref->val('name'),
    );
    return $rec[$name] == 'UPS';
  }
}