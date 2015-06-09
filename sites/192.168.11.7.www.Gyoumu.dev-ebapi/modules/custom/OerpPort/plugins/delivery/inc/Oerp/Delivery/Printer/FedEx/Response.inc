<?php
namespace Oerp\Delivery\Printer\FedEx;

class Response extends \Oerp\Delivery\Printer\Abstract_Response
{
  public function isApplicable($name)
  {
    $names = \Oerp\Delivery\FedEx\Helper::getCarrierNames();
    return in_array($name, $names, true);
  }

  public function createCall()
  {
    return new \Oerp\Delivery\Printer\FedEx\Call();
  }

  private function parseOutfile($resp)
  {
    preg_match_all('/(\d*),"(.*)"/U', $resp, $m);

    $vals = array();
    foreach($m[1] as $ind => $val){
      $vals[] = array('code' => $val, 'value' => $m[2][$ind]);
    }
    return $vals;
  }

  private function loadOutfile($packid)
  {
    $h = new \Oerp\Delivery\FedEx\Helper();
    $path = sprintf('%s/out/%s', $h->getListenerPath(), $h->getFilename($packid, 'out'));
    $files = glob($path);

    if($files[0]){
      return file_get_contents($files[0]);
    }
    return false;
  }

  private function searchOutfile($out, $code)
  {
    $out = $this->parseOutfile($out);

    foreach($out as $val){
      if($val['code'] == $code){
        return $val['value'];
      }
    }
    return false;
  }

  public function getResp($packid)
  {
    if(!$out = $this->loadOutfile($packid)){
      return array(
        'success' => null,
        'msg' => 'Error: No response yet. Try to reload this page.',
      );
    }

    if(($tnum = $this->searchOutfile($out, '1123')) !== false){
      return array(
        'success' => true,
        'msg' => 'Printed: ' . $tnum,
        'date' => $this->searchOutfile($out, '24'),
        'tid' => $packid,
        'tnum' => $tnum,
      );
    }

    if(($error = $this->searchOutfile($out, '3')) !== false) {
      return array('success' => false, 'msg' => 'Error: ' . $error);
    }
  }
}