<?php
namespace Yauc\Driver\Command;

abstract class _Abstract extends \Gulei\Command\WithReference\_Abstract
{
  protected $mgr;
  protected $drv;

  abstract protected function exe_internal();

  public function execute($refVar = null)
  {
    list($this->mgr, $this->drv) = $refVar->vals('mgr', 'drv');
    return $this->exe_internal();
  }
}