<?php
namespace Oerp\Product\Installer\Tree;

class PackageType extends \Oerp\Installer\View\Abstract_Tree
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getName()
  {
    return 'x_product.ul.tree';
  }

  protected function getModel()
  {
    return 'product.ul';
  }
}