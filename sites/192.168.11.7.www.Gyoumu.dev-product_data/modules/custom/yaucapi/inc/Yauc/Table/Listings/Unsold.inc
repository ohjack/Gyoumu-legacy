<?php
namespace Yauc\Table\Listings;

class Unsold extends _Abstract
{
  function __construct($sellerId, $param = array()){
    $arch = \Gulei\File\Helper::getPathContent('module', 'yaucapi', '/xml/sellerAuctionsClosed.xml');
    parent::__construct($sellerId, $arch, $param);

    $where = <<<SQL
`seller_id`="{$this->sellerId}" AND `status` IN (%s,%s)
SQL;

    $this->where_sql = sprintf($where, YAPI_AUC_CLOSE, YAPI_AUC_CLOSE_IGNORE);
    $this->select_sql = 'seller_id, auc_id, title, auc_url, img_url, init_price, cur_price, shipping_fee, time_end, sku, winner, src';
    $this->record_sql = <<<SQL
SELECT {$this->select_sql}
FROM {yaucapi_auction}
WHERE {$this->where_sql}
ORDER BY `time_end` DESC
SQL;

    $this->count_sql = "SELECT COUNT(*) FROM {yaucapi_auction} WHERE {$this->where_sql}";
  }

  public function getRecords($recs = null, $param = array()) {
    $param = $this->getParam(null, $param);

    $rs = pager_query($this->record_sql, $param['items_per_page'], 0, $this->count_sql);
    $recs = array();

    while ($data = db_fetch_array($rs)) {
      $recs[] = $data;
    }

    return $recs;
  }
}