<?php
namespace Oerp\Command;

class getUpsTrack extends \Gulei\Command\WithReference\WithTable
{
  public function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');

    if($rec['carrier_id'] == 'UPS' && $rec[$name] == '_PRINT_'){
      $UpsCtrlr = new \Oerp\Printer\Ups($rec['name']);

      if(!$UpsCtrlr->isSent()){
        return '<span class="gu_value">Waiting...</span>';
      }

      if($UpsCtrlr->isPrint()) {
        $trackNums = $UpsCtrlr->getTrackNum();

        if ($trackNums) {
          $t_num = implode("\n", $trackNums);

          \Oerp\Query\Basic::_execute(
            array(
                 'stock.picking',
                 'write',
                 (int)$rec['id'],
                 array('x_tnum' => $t_num)
            )
          );
          return '<span class="gu_value">' . nl2br($t_num) . '</span>';
        }
      }
      else {
        $msg = $UpsCtrlr->getLogMessage();

        if($msg == '_PRINT_'){
          return '<span class="notice">WAITING...</span>';
        }
        else{
          return sprintf(
            '<div><span class="invalid">ERROR</span><span class="show-tooltip-text">%s</span></div>',
            str_replace("\n", '<br/><br/>', $msg)
          );
        }
      }
    }
    return false;
  }
}