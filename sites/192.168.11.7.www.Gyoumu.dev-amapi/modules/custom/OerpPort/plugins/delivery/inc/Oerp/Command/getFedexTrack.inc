<?php
namespace Oerp\Command;

class getFedexTrack extends \Gulei\Command\WithReference\_Abstract
{
  public function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');

    $ref = array('rec' => $rec, 'name' => 'carrier_id');
    $isCarrierFedex = new isCarrierFedex($ref);

    if($isCarrierFedex->execute()
       && $rec[$name] == '_PRINT_'){

      $resp = _fedprint_getRec($rec["name"], "tid");

      if($resp->tnum){
        \Oerp\Query\Basic::_execute(
          array(
               'stock.picking',
               'write',
               (int)$rec['id'],
               array('x_tnum' => $resp->tnum)
          )
        );
        return $resp->tnum;
      }
      else {
        return '<span class="gu_value">Waiting...</span>';
      }
    }
    else{
      return false;
    }
  }
}