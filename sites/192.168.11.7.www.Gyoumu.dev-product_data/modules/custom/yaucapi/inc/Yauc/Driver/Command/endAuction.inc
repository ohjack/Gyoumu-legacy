<?php
namespace Yauc\Driver\Command;

class endAuction extends \Gulei\Command\WithReference\_Abstract
{
  public function execute($refVar = null)
  {
    $auc = $this->ref->val('auc');
    list($mgr, $drv) = $refVar->vals('mgr', 'drv');

    if($auc['auc_url']){
      $resp = $drv->endAuctionByLink($auc['auc_url']);
    }
    else{
      $resp = $drv->endAuctionById($auc['auc_id']);
    }

    $auc_msg = sprintf('%s [%s]', $auc['auc_id'], $auc['auc_url']);

    return $resp
        ? array('success' => true, 'msg' => $auc_msg)
        : array('success' => false, 'msg' => $auc_msg);
  }
}