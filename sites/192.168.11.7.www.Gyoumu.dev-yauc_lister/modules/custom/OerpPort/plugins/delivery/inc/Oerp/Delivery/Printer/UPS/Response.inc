<?php
namespace Oerp\Delivery\Printer\UPS;

class Response extends \Oerp\Delivery\Printer\Abstract_Response
{
  public function isApplicable($name)
  {
    $names = array('UPS');
    return in_array($name, $names, true);
  }

  public function createCall()
  {
    return new \Oerp\Delivery\Printer\UPS\Call();
  }

  public function getResp($packid)
  {
    $UpsCtrlr = new \Oerp\Delivery\UPS\Controller($packid);

    if(!$UpsCtrlr->isSent()){
      return array('success' => null);
    }

    if($UpsCtrlr->isPrint()) {
      $trackNum = $UpsCtrlr->getTrackNum();

      if ($trackNum) {
        return array('success' => true, 'tnum' => $trackNum);
      }
    }
    else {
      $msg = $UpsCtrlr->getLogMessage();

      if($msg == '_PRINT_'){
        return array('success' => null);
      }
      else{
        return array('success' => false, 'msg' => $msg);
      }
    }
  }
}