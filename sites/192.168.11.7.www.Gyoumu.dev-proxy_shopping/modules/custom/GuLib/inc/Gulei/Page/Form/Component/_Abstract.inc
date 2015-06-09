<?php
namespace Gulei\Page\Form\Component;

abstract class _Abstract extends \Gulei\Page\Themer\_Abstract
{
  abstract protected function getEleTpl();

  protected function getCaption()
  {
    $toks = explode("\\", $this->getClassname());
    $cap = str_replace('__', '-', array_pop($toks));
    $cap = str_replace('_', ' ', $cap);
    return $cap;
  }

  public function getEle()
  {
    return array_merge(
      $this->getEleTpl(),
      $this->getEleOverrides()
    );
  }

  protected function getEleOverrides()
  {
    return array();
  }

  public function theme(&$ele)
  {
    return gulib_render($ele, $this->FormPage);
  }
}