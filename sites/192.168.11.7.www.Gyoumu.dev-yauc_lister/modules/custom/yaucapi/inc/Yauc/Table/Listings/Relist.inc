<?php
namespace Yauc\Table\Listings;

class Relist extends _Abstract
{
  function __construct($sellerId, $param = array()){
    $arch = \Gulei\File\Helper::getPathContent('module', 'yaucapi', '/xml/sellerRelistList.xml');
    parent::__construct($sellerId, $arch, $param);
  }

  public function getDefaultParam(){
    $param = parent::getDefaultParam();
    $param['pager'] = false;
    $param['type'] = 'select-many';
    return $param;
  }

  public function getRecords($recs = null, $param = null)
  {
    $sql = 'SELECT relistAnchor FROM {yaucapi_sellers} WHERE seller_id = "%s"';
    $anchors = db_result(db_query($sql, $this->sellerId));
    $title_likes = str_split(trim($anchors));

    foreach($title_likes as &$title_like){
      $title_like = '`title` LIKE "' . $title_like . '%"';
    }
    $title_likes = '(' . implode(' OR ', $title_likes) . ')';

    $sql = <<< SQL
   SELECT auc_id, title, auc_url, img_url, init_price, cur_price, bidorbuy,
          shipping_fee, time_start, time_end, sku
     FROM {yaucapi_auction}
    WHERE seller_id="%s"
      AND status IN (%d, %d)
      AND %s
    ORDER BY title ASC, time_start DESC
SQL;

    $sql = sprintf($sql, $this->sellerId, YAPI_AUC_SOLD, YAPI_AUC_CLOSE, $title_likes);
    $rs = db_query($sql);

    $recs = array();
    while ($data = db_fetch_array($rs)) {
      $CtrlFlag = new \YaucFlagController($data['auc_id']);

      if($relisted = $CtrlFlag->getAucFlags()->relisted){
        continue;
      }

      $data['title'] = mb_convert_kana($data['title'], 'a');
      $recs[] = $data;
    }
    return $recs;
  }
}