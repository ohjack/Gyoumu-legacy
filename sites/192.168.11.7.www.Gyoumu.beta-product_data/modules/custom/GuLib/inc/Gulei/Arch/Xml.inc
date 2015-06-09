<?php
namespace Gulei\Arch;

class Xml extends _Abstract
{
  protected function loadArch($Dom)
  {
    return $Dom->loadXML($this->arch);
  }
}