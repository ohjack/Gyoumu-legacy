<?php
namespace Yauc\Table\Listings;

class Ignore extends _Abstract
{
  function __construct($sellerId, $param = array()){
    $arch = \Gulei\File\Helper::getPathContent('module', 'yaucapi', '/xml/ignoreList.xml');
    parent::__construct($sellerId, $arch, $param);
  }

  public function getDefaultParam(){
    $param = parent::getDefaultParam();
    $param['pager'] = false;
    return $param;
  }

  public function getRecords($param = null)
  {
    $sql = <<< SQL
    SELECT auc_id, title, auc_url, sku
    FROM {yaucapi_auction}
    WHERE seller_id="%s" AND status IN (%s,%s)
    ORDER BY sku ASC
SQL;
    $rs = db_query($sql, $this->sellerId, YAPI_AUC_SOLD_IGNORE, YAPI_AUC_CLOSE_IGNORE);

    $recs = array();
    while($rec = db_fetch_array($rs)){
      $rec['eval:status'] = $this->getStatus($rec['auc_id']);
      $recs[] = $rec;
    }
    return $recs;
  }
}