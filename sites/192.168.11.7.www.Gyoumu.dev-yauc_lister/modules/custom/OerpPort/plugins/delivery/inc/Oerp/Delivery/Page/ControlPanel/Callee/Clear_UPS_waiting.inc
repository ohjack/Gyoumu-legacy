<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee;

class Clear_UPS_waiting extends \Oerp\Delivery\Page\ControlPanel\Callee\Abstract_Clear_waiting
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getCarriers()
  {
    return array('UPS');
  }
}