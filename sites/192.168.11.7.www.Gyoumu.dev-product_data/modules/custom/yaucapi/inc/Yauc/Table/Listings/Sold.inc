<?php
namespace Yauc\Table\Listings;

class Sold extends _Abstract
{
  function __construct($sellerId, $param = array()){
    $arch = \Gulei\File\Helper::getPathContent('module', 'yaucapi', '/xml/sellerAuctionsClosed.xml');
    parent::__construct($sellerId, $arch, $param);

    $where = <<<SQL
`seller_id`="{$this->sellerId}" AND `status` IN (%s,%s) %s
SQL;

    $get = $_GET;

    if($get['start_time'] || $get['end_time']){
      $tzTaipei = new \DateTimeZone('Asia/Taipei');
      $tzTokyo = new \DateTimeZone('Asia/Tokyo');

      $StartTime = new \DateTime($get['start_time'] . ' 00:00:00', $tzTaipei);
      $StartTime->setTimezone($tzTokyo);

      $EndTime = new \DateTime($get['end_time'] . ' 23:59:59', $tzTaipei);
      $EndTime->setTimezone($tzTokyo);

      $date_sql = sprintf(
        'AND `time_end` > "%s" AND `time_end` < "%s"',
        $StartTime->format('c'), $EndTime->format('c')
      );
    }
    else{
      $date_sql = '';
    }

    $this->where_sql = sprintf($where, YAPI_AUC_SOLD, YAPI_AUC_SOLD_IGNORE, $date_sql);
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