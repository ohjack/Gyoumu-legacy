<?php
namespace Oerp\Delivery\Printer\FedEx;

class Response extends \Oerp\Delivery\Printer\FedEx\_Abstract
{
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
    $path = sprintf('%s/out/%s', self::LISTENERPATH, $this->getFilename($packid, 'out'));
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

  public function check()
  {
    $recs = $this->prov->getData();
    $q = new \Oerp\Query\Basic('stock.picking');

    foreach($recs as $rec){
      $q->setIds($rec['id']);
      $resp = $this->getResp($rec['name']);
      $success = $resp['success'];

      if($success){
        $q->write(array('x_tnum' => $resp['tnum']));
      }
      else if($success === false){
        $q->write(array('x_tnum' => '_ERROR_'));
      }
    }
  }
}