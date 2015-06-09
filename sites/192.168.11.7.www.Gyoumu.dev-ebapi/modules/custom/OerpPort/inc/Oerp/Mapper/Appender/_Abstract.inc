<?php
namespace Oerp\Mapper\Appender;

abstract class _Abstract extends \Gulei\Command\WithReference\_Abstract
{
  abstract public function isApplicable();
  abstract protected function adapt();

  public function execute($refVar = null)
  {
    list($ind, $name) = $this->ref->vals('ind', 'name');
    $recs = &$refVar->refVar('recs');

    if($name == '_seq_'){
      $recs[$ind][] = $this->adapt();
    }
    else{
      $recs[$ind][$name] = $this->adapt();
    }
  }
}