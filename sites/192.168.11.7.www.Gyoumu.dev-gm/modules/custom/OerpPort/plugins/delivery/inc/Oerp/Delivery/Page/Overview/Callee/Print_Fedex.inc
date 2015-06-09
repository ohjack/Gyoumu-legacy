<?php
namespace Oerp\Delivery\Page\Overview\Callee;

class Print_Fedex extends \Oerp\Delivery\Page\Overview\Callee\Abstract_Print
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getPrinterCall($prov)
  {
    return new \Oerp\Delivery\Printer\FedEx\Call($prov);
  }

  protected function getPrinterResp($prov)
  {
    return new \Oerp\Delivery\Printer\FedEx\Response($prov);
  }

  protected function createProvider($type)
  {
    $classname = "Oerp\\Delivery\\Provider\\Ready\\FedEx\\" . ucfirst($type);
    return new $classname(array(), array('id', 'name'));
  }

  protected function getWaitingCallee()
  {
    return 'Clear_FedEx_waiting';
  }
}