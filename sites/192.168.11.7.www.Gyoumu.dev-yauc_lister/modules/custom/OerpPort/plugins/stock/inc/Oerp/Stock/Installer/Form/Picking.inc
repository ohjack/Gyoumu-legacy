<?php
namespace Oerp\Stock\Installer\Form;

class Picking extends \Oerp\Installer\View\Abstract_Form
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getModel()
  {
    return 'stock.picking';
  }
}