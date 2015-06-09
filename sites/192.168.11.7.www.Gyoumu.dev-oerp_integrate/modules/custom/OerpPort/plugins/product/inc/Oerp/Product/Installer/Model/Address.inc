<?php
namespace Oerp\Product\Installer\Model;

class Address extends \Oerp\Installer\Model\_Abstract
{
  protected function getModel()
  {
    return 'res.partner.address';
  }

  protected function getInstFields()
  {
    return array(
      $this->createInstField('x_im', 'Instant Message', 'char'),
      $this->createInstField('x_title', 'Title', 'char'),
      $this->createInstField('x_state', 'State', 'char'),
    );
  }
}