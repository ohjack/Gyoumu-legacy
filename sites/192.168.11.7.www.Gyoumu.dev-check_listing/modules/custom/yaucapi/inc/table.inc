<?php

class YaucTable extends GuSelectTable
{
  protected $sellerId;

  protected  $select_sql;
  protected  $where_sql;
  protected  $record_sql;
  protected  $count_sql;

  function __construct($sellerId, $arch, $param = array()){
    $this->sellerId = $sellerId;
    $y = new GuYapi($sellerId);
    $y->checkAccessToken();
    parent::__construct($arch, null, $param);
  }

  protected function getFuncPrefix()
  {
    return array_merge(array('yaucapiFunc_'), parent::getFuncPrefix());
  }

  public function getRecordsCount(){
    $rs = db_query($this->count_sql);
    return db_result($rs);
  }

  public function getDefaultParam(){
    $param = parent::getDefaultParam();
    $param['mark_translated'] = false;
    $param['pager'] = true;
    $param['items_per_page'] = 80;
    return $param;
  }

  protected function checkFuncName($src, $prefix = 'yaucapiFunc_'){
    $func_name = parent::checkFuncName($src, $prefix);

    if($func_name === false){
      return parent::checkFuncName($src);
    }
    else{
      return $func_name;
    }
  }

  protected function getSelectionValue($rec, $ind){
    return $rec['auc_id'];
  }

  protected function getStatus($auc_id){
    $CtrlFlag = new YaucFlagController($auc_id);
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
}

class YaucActiveTable extends YaucTable
{
  function __construct($sellerId, $param = array()){
    $arch = GuFile::getPathContent('module', 'yaucapi', '/xml/sellerAuctionsActive.xml');
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

class YaucSoldTable extends YaucTable
{
  function __construct($sellerId, $param = array()){
    $arch = GuFile::getPathContent('module', 'yaucapi', '/xml/sellerAuctionsClosed.xml');
    parent::__construct($sellerId, $arch, $param);

    $where = <<<SQL
`seller_id`="{$this->sellerId}" AND `status` IN (%s,%s) %s
SQL;

    $get = $_GET;

    if($get['start_time'] || $get['end_time']){
      $tzTaipei = new DateTimeZone('Asia/Taipei');
      $tzTokyo = new DateTimeZone('Asia/Tokyo');

      $StartTime = new DateTime($get['start_time'] . ' 00:00:00', $tzTaipei);
      $StartTime->setTimezone($tzTokyo);

      $EndTime = new DateTime($get['end_time'] . ' 23:59:59', $tzTaipei);
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

class YaucUnsoldTable extends YaucTable
{
  function __construct($sellerId, $param = array()){
    $arch = GuFile::getPathContent('module', 'yaucapi', '/xml/sellerAuctionsClosed.xml');
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

class YaucRelistTable extends YaucTable
{
  function __construct($sellerId, $param = array()){
    $arch = GuFile::getPathContent('module', 'yaucapi', '/xml/sellerRelistList.xml');
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
    $anchor = db_result(db_query($sql, $this->sellerId));

    $sql = <<< SQL
   SELECT auc_id, title, auc_url, img_url, init_price, cur_price, bidorbuy,
          shipping_fee, time_start, time_end, sku
     FROM {yaucapi_auction}
    WHERE seller_id="%s"
      AND status IN (%d, %d)
      AND title LIKE "%s%%"
    ORDER BY title ASC, time_start DESC
SQL;

    $sql = sprintf($sql, $this->sellerId, YAPI_AUC_SOLD, YAPI_AUC_CLOSE, $anchor);
    $rs = db_query($sql);

    $recs = array();
    while ($data = db_fetch_array($rs)) {
      $CtrlFlag = new YaucFlagController($data['auc_id']);
      if($CtrlFlag->getAucFlags()->relisted)
        continue;

      $data['title'] = mb_convert_kana($data['title'], 'a');
      $recs[] = $data;
    }

    return $recs;
  }
}

class YaucIgnoreTable extends YaucTable
{
  function __construct($sellerId, $param = array()){
    $arch = GuFile::getPathContent('module', 'yaucapi', '/xml/ignoreList.xml');
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

