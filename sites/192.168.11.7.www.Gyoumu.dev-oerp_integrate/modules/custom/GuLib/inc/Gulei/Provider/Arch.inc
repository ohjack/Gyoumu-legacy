<?php
namespace Gulei\Provider;

class Arch implements _Interface
{
  private $Arch;

  function __construct($arch)
  {
    $this->Arch = new \Gulei\Arch\Element($arch);
  }

  public function getData()
  {
    return array($this->Arch->extractAttr('string', 'name'));
  }

  public function getReference()
  {
    return array('_arch' => $this->Arch);
  }

  public function count()
  {
    return $this->Arch->xp('//subject')->length;
  }
}