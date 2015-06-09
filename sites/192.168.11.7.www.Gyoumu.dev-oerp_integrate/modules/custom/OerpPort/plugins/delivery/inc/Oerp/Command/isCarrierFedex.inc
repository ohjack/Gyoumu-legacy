<?php
namespace Oerp\Command;

class isCarrierFedex extends \Gulei\Command\WithReference\_Abstract
{
  public function execute($refVar = null)
  {
    list($rec, $name) = array(
      $this->ref->val('rec'),
      $this->ref->val('name'),
    );
    return preg_match('/^Fedex/', $rec[$name]);
  }
}