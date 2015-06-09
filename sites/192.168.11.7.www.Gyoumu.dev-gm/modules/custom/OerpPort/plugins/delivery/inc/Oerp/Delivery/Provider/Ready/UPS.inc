<?php
namespace Oerp\Delivery\Provider\Ready;

class UPS extends \Oerp\Delivery\Provider\Ready\_Abstract
{
  protected function getCarrierNames()
  {
    return array('UPS');
  }
}