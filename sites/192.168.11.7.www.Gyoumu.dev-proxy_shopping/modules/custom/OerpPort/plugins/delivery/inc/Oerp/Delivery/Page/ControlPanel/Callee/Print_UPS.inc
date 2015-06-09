<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee;

class Print_UPS extends \Oerp\Delivery\Page\ControlPanel\Callee\Abstract_Print
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getCarrierNames()
  {
    return array('UPS');
  }

  protected function getPrinterCall()
  {
    return new \Oerp\Delivery\Printer\UPS\Call();
  }

  protected function getPrinterResp()
  {
    return new \Oerp\Delivery\Printer\UPS\Response();
  }

  protected function getWaitingCallee()
  {
    return 'Clear_UPS_waiting';
  }
}