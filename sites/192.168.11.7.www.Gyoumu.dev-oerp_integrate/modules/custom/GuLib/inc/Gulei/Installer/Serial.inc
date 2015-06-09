<?php
namespace Gulei\Installer;

class Serial
{
  protected $prefix;
  protected $insts;

  public function __construct($insts, $prefix = '')
  {
    $this->prefix = $prefix;
    $this->insts = $insts;
  }

  public function exe()
  {
    foreach($this->insts as $inst) {
      $classname = $this->prefix . $inst;
      $Installer = new $classname();
      $Installer->exe();
    }
  }
}