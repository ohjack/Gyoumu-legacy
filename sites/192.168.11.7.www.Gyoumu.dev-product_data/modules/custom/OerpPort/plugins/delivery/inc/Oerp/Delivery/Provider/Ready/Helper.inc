<?php
namespace Oerp\Delivery\Provider\Ready;

class Helper
{
  public function createProvider($carriers, $type, $flds)
  {
    $classname = "Oerp\\Delivery\\Provider\\Ready\\ByCarrier\\" . ucfirst($type);
    return new $classname($carriers, $flds);
  }
}