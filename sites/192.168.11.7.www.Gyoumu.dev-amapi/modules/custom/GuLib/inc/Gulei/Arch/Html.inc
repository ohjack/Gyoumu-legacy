<?php
namespace Gulei\Arch;

class Html extends _Abstract
{
  protected function loadArch($Dom)
  {
    return $Dom->loadHTML($this->arch);
  }
}