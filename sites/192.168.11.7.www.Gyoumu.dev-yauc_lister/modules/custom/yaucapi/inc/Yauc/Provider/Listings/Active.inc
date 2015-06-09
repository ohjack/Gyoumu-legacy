<?php
namespace Yauc\Provider\Listings;

class Active implements \Gulei\Provider\_Interface
{
  const AUC_NEW = 1;
  const AUC_LOADED = 2;
  const AUC_SOLD = 3;
  const AUC_CLOSE = 4;
  const AUC_ERROR = 5;
  const AUC_SOLD_IGNORE = 6;
  const AUC_CLOSE_IGNORE = 7;
  const AUC_DEAD = 8;

  const PAGER_ITEMS = 80;

  protected $sellerId;

  public function __construct($sellerId)
  {
    $this->sellerId = $sellerId;
  }

  protected function getSqlWhere()
  {
    $where = sprintf(
      '`seller_id`="%s" AND `status` IN (%s,%s)',
      $this->sellerId, self::AUC_NEW, self::AUC_LOADED
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

  protected function getSqlSelect()
  {
    return 'seller_id, auc_id, title, auc_url, img_url, init_price, cur_price, shipping_fee, time_end, sku, src';
  }

  protected function getSqlRecord()
  {
    return <<<SQL
SELECT {$this->getSqlSelect()}
FROM {yaucapi_auction}
WHERE {$this->getSqlWhere()}
ORDER BY `time_end` ASC
SQL;
  }

  protected function getSqlCount()
  {
    return "SELECT COUNT(*) FROM {yaucapi_auction} WHERE {$this->getSqlWhere()}";
  }

  protected function getStatus($auc_id){
    $CtrlFlag = new \YaucFlagController($auc_id);
    $flags = $CtrlFlag->getAucFlags();

    if($flags->to_end && $flags->to_kill == YAPI_FLAG_TOKILL){
      return 'Ending & To Kill';
    }

    if($flags->to_end){
      return 'Ending';
    }

    if($flags->to_kill == YAPI_FLAG_TOKILL){
      return 'To Kill';
    }

    if($flags->to_kill == YAPI_FLAG_KILLING
       || $flags->to_kill == YAPI_FLAG_KILLING_SCHEDULED ){
      return 'Killing';
    }
  }

  protected function getItemsPerPage()
  {
    return $this->count();
  }

  public function getData()
  {
    $paramPager = \Gulei\Pager\Basic::getPagerParam(array(
      'items_per_page' => $this->getItemsPerPage(),
      'count' => $this->count(),
    ));

    $rs = db_query_range($this->getSqlRecord(), $paramPager->from, $paramPager->items);
    $recs = array();

    while($data = db_fetch_array($rs)){
      $src = unserialize($data['src']);
      $data['carrier'] = $src['Shipping']['Method']['Name'];
      unset($data['src']);

      $data['eval:status'] = $this->getStatus($data['auc_id']);
      $recs[] = $data;
    }
    return $recs;
  }

  public function count()
  {
    $rs = db_query($this->getSqlCount());
    return db_result($rs);
  }

  public function getReference()
  {
    return null;
  }
}