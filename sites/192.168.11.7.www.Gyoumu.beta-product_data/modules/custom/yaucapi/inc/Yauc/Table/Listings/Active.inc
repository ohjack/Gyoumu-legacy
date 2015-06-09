<?php
namespace Yauc\Table\Listings;

class Active extends _Abstract
{
  function __construct($sellerId, $param = array()){
    $arch = \Gulei\File\Helper::getPathContent('module', 'yaucapi', '/xml/sellerAuctionsActive.xml');
    $param['type'] = 'select-many';
    parent::__construct($sellerId, $arch, $param);

    $this->select_sql = 'seller_id, auc_id, title, auc_url, img_url, init_price, cur_price, shipping_fee, time_end, sku';
    $this->record_sql = <<<SQL
SELECT {$this->select_sql}
FROM {yaucapi_auction}
WHERE {$this->getSqlWhere()}
ORDER BY `time_end` ASC
SQL;

    $this->count_sql = "SELECT COUNT(*) FROM {yaucapi_auction} WHERE {$this->getSqlWhere()}";
  }

  protected function getSqlWhere(){
    $where = sprintf(
      '`seller_id`="%s" AND `status` IN (%s,%s)',
      $this->sellerId, YAPI_AUC_NEW, YAPI_AUC_LOADED
    );

    $ex = '';
    foreach ($_GET as $fld_name => $keyword) {
      if(!preg_match('/(^form_|^submit:|^q$|^page$|^sel-|destination)/', $fld_name)){
        if(strlen($keyword) > 0){
          $ex .= sprintf(' AND UPPER(`%s`) LIKE "%%%s%%"', $fld_name, strtoupper($keyword));
        }
      }
    }

    return $where . $ex;
  }

  public function getRecords($param = null)
  {
    $paramPager = $this->createPager()->getParam();
    $rs = db_query_range($this->record_sql, $paramPager->from, $paramPager->items);
    $recs = array();

    while($data = db_fetch_array($rs)){
      $data['eval:status'] = $this->getStatus($data['auc_id']);
      $recs[] = $data;
    }
    return $recs;
  }
}