<?php
namespace Oerp\Installer\View;

abstract class Abstract_Form extends \Oerp\Installer\View\_Abstract
{
  protected function getDefaultParam()
  {
    return array_merge(parent::getDefaultParam(), array('type' => 'form'));
  }

  protected function getName()
  {
    return 'x_' . $this->getModel() . '.form';
  }
}