<?php
namespace Oerp\Command;

class isCarrierFedex extends \Gulei\Command\WithReference\WithTable
{
  public function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');
    return strpos($rec[$name], 'Fedex') === 0;
  }
}