<?php
namespace Oerp\Delivery\Provider\Ready;

class Fedex extends \Oerp\Delivery\Provider\Ready\_Abstract
{
  protected function getCarrierNames()
  {
    return array('Fedex IE', 'Fedex IP');
  }
}