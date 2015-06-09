<?php
namespace Oerp\Product\Installer\Form;

class Product extends \Oerp\Installer\View\Abstract_Form
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getName()
  {
    return 'x_product.product.form';
  }

  protected function getModel()
  {
    return 'product.product';
  }
}