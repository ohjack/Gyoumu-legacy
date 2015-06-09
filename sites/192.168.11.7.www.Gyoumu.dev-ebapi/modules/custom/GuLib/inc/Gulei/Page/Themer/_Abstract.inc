<?php
namespace Gulei\Page\Themer;

abstract class _Abstract
{
  protected $FormPage;

  abstract protected function getClassname();
  abstract public function theme(&$ele);

  public function __construct(\Gulei\Page\Form\_Abstract $FormPage)
  {
    $this->FormPage = $FormPage;
  }

  public function getEleName()
  {
    return 'ns:' . str_replace("\\", ':', $this->getClassname());
  }
}