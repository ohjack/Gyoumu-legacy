<?php
namespace Yauc\Driver;

class Manager extends \Yauc\Driver\Manager\_Abstract
{
  private $onlineAucs;
  private $closedAucs;

  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function createDriver()
  {
    return new \Yauc\Driver\Basic($this->sel_id, $this->pwd);
  }

  protected function getLocalAuc($auc_id, $flds = null){
    if(is_null($flds)){
      $flds = '*';
    }else{
      $flds = implode(',', $flds);
    }

    $rs = db_query('
      SELECT %s FROM {yaucapi_auction}
      WHERE auc_id="%s"
    ', $flds, $auc_id);

    return db_fetch_object($rs);
  }

  protected function removeLocalAuc($auc_id){
    $rec = new \stdClass();
    $rec->auc_id = $auc_id;
    $rec->status = YAPI_AUC_DEAD;
    drupal_write_record('yaucapi_auction', $rec, 'auc_id');
  }

  protected function loadLocalAucs($status = null){
    if(is_null($status)){
      $state_cri = '';
    }
    else if(is_array($status)){
      $state_cri = sprintf(
        'AND status IN(%s)', implode(', ', $status)
      );
    }
    else {
      $state_cri = 'AND status=' . $status;
    }

    $sql = <<< SQL
      SELECT auc_id, status FROM {yaucapi_auction}
      WHERE seller_id="%s" %s
SQL;

    $rs = db_query($sql, $this->sel_id, $state_cri);

    $aucs = array();
    while($data = db_fetch_object($rs)){
      $aucs[$data->auc_id] = $data->status;
    }

    return $aucs;
  }

  protected function getLocalActiveAucs(){
    return $this->loadLocalAucs(YAPI_AUC_NEW);
  }

  protected function loadOnlineAucs(){
    $y = new \GuYapi($this->sel_id);
    $resp = $y->getAllOnlineItems();

    if($resp === false){
      watchdog('error', 'Yaucapi: Failed to get online items of seller @sel.', array(
        '@sel' => $this->sel_id));

      return false;
    }

    $OnlineItems = new \Gulei\_Array\Basic($resp);
    $this->onlineAucs = (array)$OnlineItems->promote('AuctionID', true);

    watchdog('yaucapi', 'Loaded @n online items of seller @sel: @list', array(
      '@n' => count($resp),
      '@sel' => $this->sel_id,
      '@list' => implode(', ', array_keys($this->onlineAucs)),
    ));

    return true;
  }

  protected function loadClosedAucs(){
    $y = new \GuYapi($this->sel_id);
    $closed = array();

    foreach(array('sold', 'not_sold') as $type){
      switch($type){
        case 'sold':
          $status = YAPI_AUC_SOLD;
          break;

        case 'not_sold':
          $status = YAPI_AUC_CLOSE;
          break;
      }

      $resp = $y->getAllCloseItems($type);

      if(is_null($resp)){
        $closed[$type] = array();
        continue;
      }

      if($resp === false){
        watchdog('yaucapi', 'Failed to get closed items.', array(), WATCHDOG_ERROR);
        return false;
      }

      if(isset($resp['AuctionID'])){
        $resp = array($resp);
      }

      $items = new \Gulei\_Array\Basic($resp);
      $items = $items->promote('AuctionID', true);

      $closed[$type] = (array)$items;

      watchdog('yaucapi', 'Loaded @n @type items of seller @sel: @list', array(
        '@n' => count($closed[$type]),
        '@type' => $type,
        '@sel' => $this->sel_id,
        '@list' => implode(', ', array_keys($closed[$type])),
      ));
    }
    $this->closedAucs = $closed;
    return true;
}

  protected function loadLocalAucDetails($LIMIT, $status = null){
    $Seller = new \GuYapi($this->sel_id);

    if(!$Seller->getAppID()){
      watchdog('yaucapi', 'No AppID provided for seller @sel',
        array('@sel' => $this->sel_id), WATCHDOG_ERROR);

      return null;
    }

    $sql = <<<SQL
SELECT * FROM {yaucapi_auction}
WHERE seller_id="%s" %s AND (src IS NULL OR sku="UNKNOWN")
SQL;

    $rs = db_query_range(
      $sql,
      $this->sel_id, is_null($status) ? '' : 'AND status=' . $status,
      0, $LIMIT
    );

    $ids = array();
    while($data = db_fetch_object($rs)){
      $resp = $Seller->getAuctionItem($data->auc_id);
      $auc = $resp['ResultSet']['Result'];

      if(!$auc && isset($resp['Error'])){
        $rec = $this->getLocalAuc($data->auc_id);
        $rec->src = '';
        $rec->description = '_ERROR_';

        drupal_write_record('yaucapi_auction', $rec, 'auc_id');

        watchdog('yaucapi', 'Failed to fetch details of auction @auc_id, marked as error: [@code]@msg',
          array(
            '@auc_id' => $data->auc_id,
            '@code' => $resp['Error']['Code'],
            '@msg' => $resp['Error']['Message'],
          ), WATCHDOG_ERROR
        );
        continue;
      }

      $ids[] = $data->auc_id;

      $Rec = $this->getLocalAuc($data->auc_id);
      $Rec->qty = $auc['Quantity'];
      $Rec->init_price = $auc['Initprice'];
      $Rec->cur_price = $auc['Price'];

      $shipping = $auc['Shipping']['Method'];
      $Rec->shipping_fee = isset($shipping[0]['SinglePrice'])
          ? $shipping[0]['SinglePrice'] : $shipping['SinglePrice'];

      if(is_null($Rec->shipping_fee)){
        $Rec->shipping_fee = -1;
      }

      $Rec->bids = $auc['Bids'];
      $Rec->time_start = $auc['StartTime'];

      if((int)$Rec->status != YAPI_AUC_SOLD){
        $Rec->time_end = $auc['EndTime'];
      }

      $Rec->category_id = $auc['CategoryID'];
      $Rec->category_path = $auc['CategoryPath'];
      $Rec->item_status = $auc['ItemStatus']['Condition'];
      $Rec->src = serialize($auc);
      $Rec->description = $auc['Description'];
      $Rec->bidorbuy = $auc['Bidorbuy'];

      drupal_write_record('yaucapi_auction', $Rec, 'auc_id');

      $RecCtrl = new \YaucRecord($Rec->auc_id);
      $RecCtrl->updateSku();
    }
    watchdog('yaucapi', 'Loaded @n auction details of seller @sel: @list',
      array('@n' => count($ids), '@sel' => $this->sel_id,
            '@list' => implode(', ', $ids))
    );
  }

  public function loadAucEndtime(){
    $LIMIT = 30;
    $sql = <<< SQL
    SELECT auc_id FROM {yaucapi_auction}
    WHERE
      seller_id = "%s"
      AND status IN (%s,%s)
      AND (time_end IS NULL OR time_end="")
      AND winner IS NOT NULL
SQL;

    $rs = db_query_range($sql, $this->sel_id, YAPI_AUC_SOLD, YAPI_AUC_SOLD_IGNORE, 0, $LIMIT);
    $q = new \GuYapi($this->sel_id);
    $loaded = array();

    while($auc_id = db_result($rs)){
      $param = array(
        'appid' => $q->getAppID(),
        'auctionID' => $auc_id,
      );

      $resp = $q->query('BidHistory', $param);

      $arch = new \Gulei\Arch\Element($resp);
      $date = trim(
        $arch->xp('/ResultSet/Result[last()]/Date')->item(0)->nodeValue);

      $rec = new \stdClass();
      $rec->auc_id = $auc_id;
      $rec->time_end = $date;
      drupal_write_record('yaucapi_auction', $rec, 'auc_id');

      $loaded[] = $auc_id;
    }

    watchdog('yaucapi', 'Loaded @n sold auction end time of seller @sel: @list',
      array('@n' => count($loaded), '@sel' => $this->sel_id,
            '@list' => implode(', ', $loaded))
    );
  }

  public function cron()
  {
    $this->killQueuedAuction();

    if ($this->loadOnlineAucs()) {
      $this->updateLocalActiveAucs();

      if ($this->loadClosedAucs()) {
        $this->markClosedAucs();
      }

      $this->loadLocalAucDetails(10, YAPI_AUC_SOLD);
      $this->loadLocalAucDetails(50);
      $this->loadAucEndtime();
    }

    $this->relistClosedAuctions();
    $this->endActiveAuctions();
  }

  protected function updateLocalActiveAucs(){
    $updated = array(
      'new'=> array(),
      'update' => array(),
      'close' => array(),
    );

    $online_ids = array_keys($this->onlineAucs);
    foreach($online_ids as &$each){
      $each = '"' . $each . '"';
    }

    $whereIn = empty($online_ids) ? '' : sprintf('AND auc_id NOT IN (%s)', implode(',', $online_ids));

    $sql = <<<SQL
SELECT auc_id FROM {yaucapi_auction} WHERE seller_id="%s" AND status IN (%d,%d) %s
SQL;
    $sql = sprintf($sql, $this->sel_id, YAPI_AUC_NEW, YAPI_AUC_LOADED, $whereIn);

    $rs = db_query($sql);
    while($rec = db_fetch_object($rs)){
      $rec->status = YAPI_AUC_CLOSE;
      drupal_write_record('yaucapi_auction', $rec, array('auc_id'));
      $updated['close'][] = $rec->auc_id;
    }

    foreach($this->onlineAucs as $auc_id => $item){
      $localAuc = $this->getLocalAuc($auc_id, array('status'));

      $Rec = new \stdClass();
      $Rec->status = YAPI_AUC_NEW;
      $Rec->auc_id = $auc_id;
      $Rec->seller_id = $item['SellerID'];
      $Rec->title = $item['Title'];
      $Rec->auc_url = $item['AuctionItemUrl'];
      $Rec->item_url = $item['ItemUrl'];
      $Rec->img_url = $item['Image'];
      $Rec->cur_price = $item['CurrentPrice'];
      $Rec->bidorbuy = $item['BidOrBuy'];
      $Rec->bids = $item['Bids'];
      $Rec->time_end = $item['EndTime'];

      if(!$localAuc){
        drupal_write_record('yaucapi_auction', $Rec);
        $updated['new'][] = $auc_id;
      }
      else if (!$this->isIgnored($localAuc->status)
               && $localAuc->status !== YAPI_AUC_NEW
               && $localAuc->status !== YAPI_AUC_LOADED
      ) {
        $Rec->src = null;
        $Rec->description = null;

        drupal_write_record('yaucapi_auction', $Rec, 'auc_id');
        $updated['update'][] = $auc_id;
      }
    }

    watchdog('yaucapi', 'Mark @n online record(s) as closed of seller @sel: @list', array(
      '@n' => count($updated['close']), '@sel' => $this->sel_id,
      '@list' => implode(', ', $updated['close'])
    ));

    watchdog('yaucapi', 'Added @n new record(s) of seller @sel: @list', array(
      '@n' => count($updated['new']), '@sel' => $this->sel_id,
      '@list' => implode(', ', $updated['new'])
    ));

    watchdog('yaucapi', 'Updated @n record(s) of seller @sel: @list', array(
      '@n' => count($updated['update']), '@sel' => $this->sel_id,
      '@list' => implode(', ', $updated['update'])
    ));
  }

  public function isIgnored($status){
    return $status == YAPI_AUC_CLOSE_IGNORE
           || $status == YAPI_AUC_SOLD_IGNORE;
  }

  protected function markClosedAucs(){
    //mark sold records
    foreach(array('sold', 'not_sold') as $type){
      switch($type){
        case 'sold':
          $status = YAPI_AUC_SOLD;
          break;

        case 'not_sold':
          $status = YAPI_AUC_CLOSE;
          break;
      }

      $closeMarked = array(
        'mark' => array(),
        'missing' => array(),
      );

      foreach($this->closedAucs[$type] as $auc_id => $closed){
        $rec = $this->getLocalAuc($auc_id);

//      missing record
        if(!$rec){
          $rec = new \stdClass;
          $rec->auc_id = $auc_id;
          $rec->status = $status;
          $rec->winner = $closed['Winner']['Id'];
          $rec->seller_id = $this->sel_id;
          $rec->title = $closed['Title'];
          $rec->auc_url = $closed['AuctionItemUrl'];
          $rec->item_url = $closed['Winner']['ItemListUrl'];
          $rec->img_url = $closed['Image'];
          $rec->cur_price = $closed['HighestPrice'];
          $rec->time_end = $closed['EndTime'];

          drupal_write_record('yaucapi_auction', $rec);
          $closeMarked['missing'][] = $auc_id;
        }
//      record to mark
        else {
          if(!$this->isIgnored($rec->status)){
            $rec->status = $status;
          }

          if($status == YAPI_AUC_SOLD && $this->isIgnored($rec->status)){
            $rec->status = YAPI_AUC_SOLD_IGNORE;
          }

          $rec->winner = $closed['Winner']['Id'];
          $rec->cur_price = $closed['HighestPrice'];
          $rec->time_end = $closed['EndTime'];

          drupal_write_record('yaucapi_auction', $rec, 'auc_id');
          $closeMarked['mark'][] = $auc_id;
        }
      }

      watchdog('yaucapi', 'Updated: @c @type item(s) of seller @seller: @list',
        array(
          '@c' => count($closeMarked['mark']),
          '@type' => $type,
          '@seller' => $this->sel_id,
          '@list' => implode(', ', $closeMarked['mark']),
        )
      );

      watchdog('yaucapi', 'Added: @c @type item(s) of seller @seller: @list',
        array(
          '@c' => count($closeMarked['missing']),
          '@type' => $type,
          '@seller' => $this->sel_id,
          '@list' => implode(', ', $closeMarked['missing']),
        )
      );
    }

    $this->sweepMissingClosedAuctions();
  }

  private function sweepMissingClosedAuctions(){
    $closed_ids = array_keys($this->closedAucs['not_sold']);
    foreach($closed_ids as &$each){
      $each = '"' . $each . '"';
    }

    $whereIn = empty($closed_ids) ? '' : sprintf('AND auc_id NOT IN (%s)', implode(',', $closed_ids));

    $sql = sprintf(
      'SELECT auc_id FROM {yaucapi_auction} WHERE seller_id = "%s" AND status IN (%d,%d) %s',
      $this->sel_id, YAPI_AUC_CLOSE, YAPI_AUC_CLOSE_IGNORE, $whereIn
    );

    $rs = db_query($sql);
    $swept = array();

    while($rec = db_fetch_object($rs)){
      $swept[] = '"' . $rec->auc_id . '"';
    }

    if(!empty($swept)) {
      $sql = sprintf('DELETE FROM {yaucapi_auction} WHERE auc_id IN (%s)', implode(',', $swept));
      db_query($sql);
    }

    watchdog('yaucapi', 'Swept @n missing closed record(s) of seller @sel: @list', array(
      '@n' => count($swept), '@sel' => $this->sel_id,
      '@list' => implode(', ', $swept)
    ));
  }

  protected function removeDeadAucs(){
    $died = array();
    foreach($this->loadLocalAucs() as $auc_id => $status){
      switch($status){
        case YAPI_AUC_SOLD:
        case YAPI_AUC_SOLD_IGNORE:
          if(!key_exists($auc_id, $this->closedAucs['sold'])){
            $died[] = $auc_id;
          }
          break;
        case YAPI_AUC_CLOSE:
        case YAPI_AUC_CLOSE_IGNORE:
          if(!key_exists($auc_id, $this->closedAucs['not_sold'])){
            $died[] = $auc_id;
          }
          break;
      }
    }

    foreach($died as $each){
      $this->removeLocalAuc($each);
    }

    watchdog('yaucapi', 'Mark @n record(s) of seller @sel as dead: @list', array(
      '@n' => count($died),
      '@sel' => $this->sel_id,
      '@list' => implode(', ', $died)
    ));
  }

  protected function markMissingAucs(){
    $missing = array_diff_key($this->getLocalActiveAucs(), $this->onlineAucs);

    if($missing){
      $missing = array_keys($missing);

      foreach($missing as $each){
        $rec = $this->getLocalAuc($each);
        $rec->status = YAPI_AUC_CLOSE;

        drupal_write_record('yaucapi_auction', $rec, 'auc_id');
      }

      watchdog('yaucapi', 'Marked @c missing items as closed: @list',
        array(
          '@c' => count($missing),
          '@list' => implode(', ', $missing),
        )
      );
    }
  }

  public function getAuctions($flds = null){
    if(is_null($flds)){
      $flds = '*';
    }else{
      $flds = implode(',', $flds);
    }

    $rs = db_query('
      SELECT %s FROM {yaucapi_auction}
      WHERE seller_id="%s" AND status=%d
    ', $flds, $this->sel_id, YAPI_AUC_NEW);

    $aucs = array();
    while($data = db_fetch_object($rs)){
      $aucs[] = $data;
    }

    return $aucs;
  }

  private function getSku($html){
    if(empty($html)){
      return null;
    }

    preg_match('/SKU[^[:alnum:]-]*([[:alnum:] -]*)/', $html, $m);
    if($m[1]){
      return $m[1];
    }

    preg_match("/商品番号[^[:alnum:]-]*([[:alnum:]-]*)/u", $html, $m);
    if($m[1]){
      return $m[1];
    }

    return null;
  }

  public function getOnlineSkus(){
    $aucs = $this->getAuctions(array('description'));

    $skus = array();
    foreach($aucs as $auc){
      $sku = $this->getSku($auc->description);

      if($sku){
        $skus[] = $sku;
      }
    }

    return array_unique($skus);
  }

  public function getSkus(){
    $q = new \Oerp\Query\Basic('product.product', null, array('default_code'));
    return (array)$q->raw()->collect('default_code');
  }

  public function getOfflineSkus($custom_opt = array()){
    $skus = $this->getSkus();
    $online = $this->getOnlineSkus();
    $offline = array_diff($skus, $online);
    unset($skus, $online);

    $opt = array(
      'asList' => false,
      'sort' => false,
    );

    $opt = array_merge($opt, $custom_opt);

    if($opt['sort']){
      sort($offline);
    }

    if(!$opt['asList']){
      return $offline;
    }
    else{
      $output = '<ul>';

      foreach($offline as $item){
        $output .= '<li>' . $item . '</li>';
      }

      $output .= '</ul>';

      return $output;
    }
  }

  public function relistClosedAuctions()
  {
    if(!$this->prepareDriver('Relisting')){
      return false;
    }

    $t = new \Yauc\Table\Listings\Relist($this->sel_id);
    $recs = $t->getRecords();
    $log = array(
      'success' => array(),
      'failed' => array(),
      'relisted' => array(),
    );

    foreach($recs as $rec) {
      $rs = $this->driver->relistAuction($rec['auc_url']);

      switch($rs){
        case YAUC_FLAG_SUCCESS:
          $log['success'][] = $rec['auc_id'];
          break;

        case YAUC_FLAG_RELISTED:
          $CtrlFlag = new \YaucFlagController($rec['auc_id']);
          $CtrlFlag->setRelisted(true);
          $log['relisted'][] = $rec['auc_id'];
          break;

        default:
          $log['failed'][] = $rec['auc_id'];
      }
    }

    watchdog(
      'GuYauc', "@sel_id relisting SUCCESS: @success RELISTED: @relisted FAILED: @failed",
      array(
           '@sel_id' => $this->sel_id,
           '@success' => implode(', ', $log['success']),
           '@relisted' => implode(', ', $log['relisted']),
           '@failed' => implode(', ', $log['failed'])
      )
    );

    return true;
  }

  private function queryAuctionByFlag($flag, $fields = null, $val = true)
  {
    if(is_null($fields))
      $fields = array('*');

    if(is_string($fields))
      $fields = array($fields);

    foreach($fields as &$field){
      if($field != '*')
        $field .= ' AS ' . $field;

      $field = 'a.' . $field;
    }

    $sql = <<<SQL
SELECT %s
FROM yaucapi_flag AS f, yaucapi_auction AS a
WHERE f.%s = %d AND f.auc_id = a.auc_id
SQL;

    $rs = db_query($sql, implode(',', $fields), $flag, $val);
    $queued = array();

    while($data = db_fetch_object($rs)){
      $queued[] = $data;
    }
    return $queued;
  }

  public function killQueuedAuction()
  {
    $y = new \GuYapi($this->sel_id);

    $queued = $this->queryAuctionByFlag('to_kill', 'auc_id', YAPI_FLAG_KILLING);
    $queued2 = $this->queryAuctionByFlag('to_kill', 'auc_id', YAPI_FLAG_KILLING_SCHEDULED);
    $queued = array_merge($queued, $queued2);

    if(empty($queued)){
      watchdog('GuYauc', 'No killing queued.');
      return;
    }

    $log = array(
      'success' => array(),
      'failed' => array(),
    );

    foreach($queued as $each){
      $resp = $y->deleteMyCloseList($each->auc_id);

      if($resp){
        $log['success'][] = $each->auc_id;
      }
      else{
        $log['failed'][] = $each->auc_id;
      }
    }

    watchdog(
      'GuYauc', "@sel_id killing SUCCESS: @success FAILED: @failed",
      array(
           '@sel_id' => $this->sel_id,
           '@success' => implode(', ', $log['success']),
           '@failed' => implode(', ', $log['failed'])
      )
    );
    return;
  }
}
