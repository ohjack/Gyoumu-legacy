<?php
namespace Oerp\Delivery\Printer;

abstract class Abstract_Response
{
  abstract public function isApplicable($name);
  abstract public function createCall();
  abstract public function getResp($packid);

  public function check($recs)
  {
    if(isset($recs['id'])){
      $recs = array($recs);
    }

    $q = new \Oerp\Query\Basic('stock.picking');

    foreach($recs as $rec){
      if ($rec['x_tnum'] == '_PRINT_') {
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
}