<?php
namespace Oerp\Delivery\Page\Ready\Callee;

class Print_FedEx extends \Oerp\Delivery\Page\Ready\Callee\Abstract_Print {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function getPrintCall() {
    return new \Oerp\Delivery\Printer\FedEx\Call();
  }
}