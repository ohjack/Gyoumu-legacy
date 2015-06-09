<?php
namespace Gulei\Page\Form;

abstract class Abstract_WithArch extends \Gulei\Page\Form\_Abstract
{
  abstract protected function getFilepath();

  protected function getArch()
  {
    $toks = explode("\\", $this->getClassname());
    return file_get_contents($this->getFilepath() . '/' . array_pop($toks) . '.xml');
  }
}