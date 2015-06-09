<?php
namespace Oerp\Delivery\FedEx;

class Helper
{
  public function getListenerPath()
  {
    return '/home/jeremy/FedexListener';
  }

  public function getFilename($packid, $ext)
  {
    $classname = str_replace("\\", '_', __CLASS__);
    $fname = $classname . '-' . $packid;

    if($ext !== false){
      $fname .= '.' . $ext;
    }
    return $fname;
  }

  static public function getCarrierNames()
  {
    return array('Fedex IE', 'Fedex IP');
  }
}