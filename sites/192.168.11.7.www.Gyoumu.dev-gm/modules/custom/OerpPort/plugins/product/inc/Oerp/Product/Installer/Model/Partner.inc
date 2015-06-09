<?php
namespace Oerp\Product\Installer\Model;

class Partner extends \Oerp\Installer\Model\_Abstract
{
  protected function getModel()
  {
    return 'res.partner';
  }

  protected function getInstFields()
  {
    return array(
      $this->createInstField('x_regnum', 'Register Number', 'char'),
      $this->createInstField('x_payment_note', 'Payment Note', 'text'),
    );
  }
}