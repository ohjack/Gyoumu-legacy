<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee;

class Clear_FedEx_waiting extends \Oerp\Delivery\Page\ControlPanel\Callee\Abstract_Clear_waiting
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getCarriers()
  {
    return \Oerp\Delivery\FedEx\Helper::getCarrierNames();
  }
}