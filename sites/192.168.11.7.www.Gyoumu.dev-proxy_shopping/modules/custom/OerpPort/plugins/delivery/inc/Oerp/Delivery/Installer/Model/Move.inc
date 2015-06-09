<?php
namespace Oerp\Delivery\Installer\Model;

class Move extends \Oerp\Installer\Model\_Abstract
{
  protected function getModel()
  {
    return 'stock.move';
  }

  protected function getInstFields()
  {
    return array(
      $this->createInstField('x_saleprice', 'Sale Price', 'float'),
      $this->createInstField('x_currency', 'Currency', 'char'),
    );
  }
}