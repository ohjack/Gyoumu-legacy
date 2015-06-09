<?php
namespace Oerp\Delivery\Printer\UPS;

class Call extends \Oerp\Delivery\Printer\Abstract_Call
{
  protected function _print($rec, $pkg)
  {
    $ctrl = new \Oerp\Delivery\UPS\Controller($rec['name']);
    $ctrl->clearRecord();
    return $ctrl->sendRecord($rec, $pkg);
  }
}