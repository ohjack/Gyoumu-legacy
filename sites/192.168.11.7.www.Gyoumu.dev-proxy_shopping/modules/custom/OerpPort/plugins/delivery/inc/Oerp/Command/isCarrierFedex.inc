<?php
namespace Oerp\Command;

class isCarrierFedex extends \Gulei\Command\WithReference\WithTable
{
  public function execute($refVar = null)
  {
    list($rec) = $this->ref->vals('rec');
    $nargs = $this->getCommandNamedArgs();
    return strpos($rec[$nargs['name']], 'Fedex') === 0;
  }
}