<?php
namespace Oerp\Product\Installer\Model;

class PackageType extends \Oerp\Installer\Model\_Abstract
{
  protected function getModel()
  {
    return 'product.ul';
  }

  protected function getInstFields()
  {
    return array(
      $this->createInstField('x_length', 'Length', 'char'),
      $this->createInstField('x_width', 'Width', 'char'),
      $this->createInstField('x_height', 'Height', 'char'),
    );
  }
}