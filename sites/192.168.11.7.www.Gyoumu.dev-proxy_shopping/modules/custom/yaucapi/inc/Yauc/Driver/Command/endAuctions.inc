<?php
namespace Yauc\Driver\Command;

class endAuctions extends \Gulei\Command\WithReference\_Abstract
{
  protected $resp;

  public function execute($refVar = null)
  {
    $recs = $this->ref->val('recs');
    list($mgr, $drv) = $refVar->vals('mgr', 'drv');

    if(empty($recs)){
      return array(
        'success' => false,
        'msg' => 'No ending queued.'
      );
    }

    foreach($recs as $toEnd){
      $mgr->apply(new \Yauc\Driver\Command\endAuction(array('auc' => $toEnd)));
    }
    return array(
      'success' => true,
    );
  }
}