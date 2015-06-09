<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee;

class Print_Fedex extends \Oerp\Delivery\Page\ControlPanel\Callee\Abstract_Print
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getCarrierNames()
  {
    return \Oerp\Delivery\FedEx\Helper::getCarrierNames();
  }

  protected function getPrinterCall()
  {
    return new \Oerp\Delivery\Printer\FedEx\Call();
  }

  protected function getPrinterResp()
  {
    return new \Oerp\Delivery\Printer\FedEx\Response();
  }

  protected function getWaitingCallee()
  {
    return 'Clear_FedEx_waiting';
  }
}