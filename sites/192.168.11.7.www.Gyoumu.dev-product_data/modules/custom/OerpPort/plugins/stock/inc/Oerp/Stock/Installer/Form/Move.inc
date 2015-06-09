<?php
namespace Oerp\Stock\Installer\Form;

class Move extends \Oerp\Installer\View\Abstract_Form
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getModel()
  {
    return 'stock.move';
  }

  protected function getDefaultParam() {
    $param = parent::getDefaultParam();
    $param['priority'] = 3;
    return $param;
  }
}