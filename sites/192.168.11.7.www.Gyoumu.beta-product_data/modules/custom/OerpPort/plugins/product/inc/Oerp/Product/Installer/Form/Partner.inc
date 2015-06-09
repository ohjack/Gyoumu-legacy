<?php
namespace Oerp\Product\Installer\Form;

class Partner extends \Oerp\Installer\View\Abstract_Form
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getModel()
  {
    return 'res.partner';
  }
}