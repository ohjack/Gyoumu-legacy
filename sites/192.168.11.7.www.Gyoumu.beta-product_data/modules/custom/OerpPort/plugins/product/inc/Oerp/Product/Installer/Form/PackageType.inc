<?php
namespace Oerp\Product\Installer\Form;

class PackageType extends \Oerp\Installer\View\Abstract_Form
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getName()
  {
    return 'x_product.ul.form.view';
  }

  protected function getModel()
  {
    return 'product.ul';
  }
}