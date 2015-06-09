<?php
namespace Oerp\Installer\View;

abstract class Abstract_Tree extends \Oerp\Installer\View\_Abstract
{
  protected function getDefaultParam()
  {
    return array_merge(parent::getDefaultParam(), array('type' => 'tree'));
  }

  protected function getName()
  {
    return 'x_' . $this->getModel() . '.tree';
  }
}