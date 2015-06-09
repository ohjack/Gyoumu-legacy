<?php
namespace Oerp\Product\Installer\Tree;

class Product extends \Oerp\Installer\View\Abstract_Tree
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getName()
  {
    return 'x_product.product.tree';
  }

  protected function getModel()
  {
    return 'product.product';
  }
}