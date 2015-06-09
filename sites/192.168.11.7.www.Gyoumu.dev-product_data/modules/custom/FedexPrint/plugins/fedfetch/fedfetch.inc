<?php

abstract class AbstractExportRecFetcher
{
  static protected $urls = array();

  protected $ch;
  protected $login_msg;
  protected $custId;
  protected $passWord;
  protected $selectBan;

  const DATE_CHECKED = 1;
  const REMOTE_DATA_FETCHED = 2;
  const REMOTE_DATA_EMPTY = 3;

  public function __construct($custId, $passWord, $selectBin){
    $this->custId = $custId;
    $this->passWord = $passWord;
    $this->selectBan = $selectBin;
  }

  public function getCustId()
  {
    return $this->custId;
  }

  private function init()
  {
    if(is_null($this->ch)){
      $this->ch = curl_init();
      $this->login_msg = $this->login();
    }
  }

  public function __destruct(){
    if(!is_null($this->ch)){
      curl_close($this->ch);
    }
  }

  protected function login()
  {
    $param = array(
      'belongTo' => 'FDX',
      'custId' => $this->custId,
      'passWord' => $this->passWord,
    );

    return $this->exec($this->getLoginUrl(), $param);
  }

  public function getLoginMessage()
  {
    return $this->login_msg;
  }

  protected function getLoginUrl()
  {
    return 'https://gocs.tradevan.com.tw/pgocs-bin/APGOCS/LogonAction.do';
  }

  protected function getControllerUrl()
  {
    return 'https://gocs.tradevan.com.tw/pgocs-bin/APGOCS/MessageStateAction.do';
  }

  protected function exec($url, $post, $opts = array()){
    $this->init();
    curl_setopt($this->ch, CURLOPT_POST, 1);
    curl_setopt($this->ch, CURLOPT_RETURNTRANSFER, 1);

    if(isset($opts['bin']) && $opts['bin']){
      curl_setopt($this->ch, CURLOPT_BINARYTRANSFER, 1);
    }
    else{
      curl_setopt($this->ch, CURLOPT_BINARYTRANSFER, 0);
    }

    curl_setopt($this->ch, CURLOPT_COOKIEJAR, '/tmp/cookie');
    curl_setopt($this->ch, CURLOPT_URL, $url);
    curl_setopt($this->ch, CURLOPT_POSTFIELDS, $post);

    return curl_exec($this->ch);
  }

  public function adaptDate($year, $month, $day){
    $year = $year > 1911 ? $year - 1911 : (int)$year;
    $month = $month < 10 ? '0' . (int)$month : $month;
    $day = (int)$day;
    $day = $day
      ? $day < 10
        ? '0' . $day : $day
      : '__';

    $date = $year + 1911 . $month . $day;

    return array(
      'year' => $year,
      'month' => $month,
      'day' => $day,
      'date' => $date,
    );
  }

  protected function getQueryParamTemplate($year, $month, $day)
  {
    $adaptedDate = $this->adaptDate($year, $month, $day);
    $year = $adaptedDate['year'];
    $month = $adaptedDate['month'];
    $day = $adaptedDate['day'];
    $date = $adaptedDate['date'];

    return array(
      'asType' => 'A',
      'boxNoBeg' => '',
      'boxNoEnd' => '',
      'clickFlag' =>	'true',
      'command' => 'query',
      'day1' => $day,
      'day2' => $day,
      'day3' => '',
      'day4' => '',
      'declNoBeg' => '',
      'declNoEnd' => '',
      'ediType' => '5203',
      'hawbBeg' => '',
      'hawbEnd' => '',
      'mawbBeg' => '',
      'mawbEnd' => '',
      'memoNoBeg' => '',
      'memoNoEnd' => '',
      'month1' => $month,
      'month2' =>	$month,
      'month3' => '',
      'month4' => '',
      'oneDateBeg' => $date,
      'oneDateEnd' =>	$date,
      'orderType' => 'DECL_NO',
      'ownerBan' => '',
      'processId' => '',
      'programName' => 'P0004',
      'selectBan' => $this->selectBan,
      'trackingNoBeg' => '',
      'trackingNoEnd' => '',
      'twoDateBeg' => '',
      'twoDateEnd' => '',
      'year1' => $year,
      'year2' => $year,
      'year3' => '',
      'year4' => '',
    );
  }

  protected function getDailyFirstPage($year, $month, $day)
  {
    $param = $this->getQueryParamTemplate($year, $month, $day);
    return $this->exec($this->getControllerUrl(), http_build_query($param, '', '&'));
  }

  protected function getDailyOtherPage($pageFirst)
  {
    preg_match("/<input type='hidden' name='totPage' value='(.*)'/", $pageFirst, $m);
    $totPage = $m[1];

    $pages = array();
    for($c = 2; $c <= $totPage; $c++){
      $post = "command=detail&ownerBan=&programName=P0004&processId=&transactionId=&declNo=&ediType=5203&asType=A&nowPage=$c&totPage=$totPage&goPage=";
      $resp = $this->exec($this->getControllerUrl() . '?command=toquery', $post);

      if($resp === false)
        return false;

      $pages[] = $resp;
    }
    return $pages;
  }

  public function getDailyRecIds($year, $month, $day){
    $pageFirst = $this->getDailyFirstPage($year, $month, $day);

    if(empty($pageFirst)){
      return false;
    }

    $pages = array_merge(array($pageFirst), $this->getDailyOtherPage($pageFirst));

    $ids = array();
    foreach($pages as $page){
      preg_match_all("<input type=checkbox name='strPrintId' value='(.*)' >", $page, $m);
      $ids = array_merge($ids, $m[1]);
    }
    return $ids;
  }

  public function getRaw($ids){
    if(!$ids || !is_array($ids)){
      return false;
    }

    $raw = array();
    foreach($ids as $id){
      $id_send = urlencode($id);
      $post = 'strPrintId=' . $id_send;
      $post = "command=PntDeclHTM&ownerBan=&programName=P0004&processId=&transactionId=&declNo=&ediType=5203&asType=A&$post&nowPage=&totPage=&goPage=";
      $raw[$id]['html'] = $this->exec($this->getControllerUrl(), $post);

      $post = "command=download&ownerBan=&programName=P0004&processId=&transactionId=&declNo=&ediType=5203&asType=A&$post&nowPage=&totPage=&goPage=";
      $raw[$id]['txt'] = $this->exec($this->getControllerUrl(), $post);
    }
    return $raw;
  }

  public function getPdfBin($ids){
    if(!$ids || !is_array($ids)){
      return false;
    }

    $id_send = array();
    foreach($ids as $id){
      $id_send[] = 'strPrintId=' . urlencode($id);
    }

    $post = sprintf("command=PrintPDF&ownerBan=&programName=P0004&processId=&transactionId=&declNo=&ediType=5203&asType=A&%s&nowPage=&totPage=&goPage="
      , implode('&', $id_send));

    return $this->exec($this->getControllerUrl(), $post, array('bin' => true));
  }

  public function savePdfFile($ids, $date){
    $basedir = '/home/jeremy/CustomExportReport';
    $fname = sprintf('%s/%s-%s.pdf', $basedir, $this->custId, $date);
    $bin = $this->getPdfBin($ids);
    return $bin === false ? false : file_put_contents($fname, $bin);
  }

  public function parseRaw($ids, $src){
    foreach($ids as &$id){
      preg_match('/([\d]{14}).*  (.*)/', $id, $m);

      $id = array('id' => $m[2], 'key' => $m[1]);
    }
    unset($id);

    $recs = array();
    foreach($ids as $id){
      $pat = '/.' . $id['key'] . '.*/';
      preg_match_all($pat, $src, $m);

      $recs[$id['id']] = implode("\n", $m[0]);
    }

    return $recs;
  }

  public function getDailyRaw($year, $month, $day){
    $ids = $this->getDailyRecIds($year, $month, $day);
    return $this->getRaw($ids);
  }

  public function checkDayGot($year, $month, $day){
    $date = $this->adaptDate($year, $month, $day);

    $rs = db_query('
      SELECT date FROM {fedfetch_export_dailycheck}
      WHERE `date`="%s" AND `account`="%s"
    ', $date['date'], $this->custId);

    return db_result($rs);
  }

  public function checkDay($date){
    $chk = new stdClass();
    $chk->date = $date;
    $chk->account = $this->custId;
    drupal_write_record('fedfetch_export_dailycheck', $chk);
  }

  public function saveDailyRaw($year, $month, $day){
    if($this->checkDayGot($year, $month, $day)){
      return self::DATE_CHECKED;
    }

    $date = $this->adaptDate($year, $month, $day);
    $raw = $this->getDailyRaw($year, $month, $day);

    if($raw === false){
      return false;
    }

    if(!$raw){
      $this->checkDay($date['date']);
      return self::REMOTE_DATA_EMPTY;
    }

    $rows = new \Gulei\_Array\Basic($raw);
    unset($raw);

    foreach($rows as $key => $row){
      preg_match('/([\d]{14}).*  (.*)/', $key, $m);

      $rec = new stdClass();
      $rec->pid = $key;
      $rec->html = $row['html'];
      $rec->txt = $row['txt'];
      $rec->date = $date['date'];
      $rec->account = $this->custId;

      drupal_write_record('fedfetch_export_recs', $rec);
    }

    $this->savePdfFile($this->getDailyRecIds($year, $month, $day), $date['date']);
    $this->checkDay($date['date']);
    return self::REMOTE_DATA_FETCHED;
  }

  public function fetchRecords($date){
    $rs = db_query('
      SELECT rid,pid,html,txt,date FROM {fedfetch_export_recs}
      WHERE `date` LIKE "%s" AND `account` = "%s"
      ORDER BY `date`, `pid`
    ', $date, $this->custId);

    $recs = array();
    while($data = db_fetch_array($rs)){
      $recs[] = $data;
    }

    return $recs;
  }

  public function delDayRecord($year, $month, $day){
    $date = $this->adaptDate($year, $month, $day);

    db_query('
      DELETE FROM {fedfetch_export_recs}
      WHERE `date` LIKE "%s" AND `account` = "%s"
    ', $date['date'], $this->custId);

    db_query('
      DELETE FROM {fedfetch_export_dailycheck}
      WHERE `date` LIKE "%s" AND `account` = "%s"
    ', $date['date'], $this->custId);
  }

  public function getDayRecord($year, $month, $day, $force = false){
    $date = $this->adaptDate($year, $month, $day);

    if(!$this->checkDayGot($year, $month, $day) || $force){
      $this->delDayRecord($year, $month, $day);
      $this->saveDailyRaw($year, $month, $day);
    }

    return $this->fetchRecords($date['date']);
  }

  public function getMonthRecord($year, $month){
    $date = $this->adaptDate($year, $month, 0);
    return $this->fetchRecords($date['date']);
  }

  public function parseRecords($recs){
    foreach($recs as $key => $rec){
      $src = $rec['txt'];
      $src = explode("\n", $src);
      $src[0] = sscanf($src[0], '%15[ -~]%14[ -~]%28[ -~]%12s    %7[ -~]%s%s%s%s%9[ -~]%9[ -~]%s%s%s');
      $src[1] = explode("\t", preg_replace('/[ ]{2,}/', "\t", $src[1]));
      $src[2] = explode("\t", preg_replace('/[ ]{2,}/', "\t", $src[2]));

      $recs[$key]['item'] = $src[1][1];
      $recs[$key]['piece'] = $src[1][3];
      $recs[$key]['fob'] = floatval($src[0][12]);
      $recs[$key]['sh.fee'] = floatval($src[0][9]);
      $recs[$key]['insur'] = floatval($src[0][10]);
      $recs[$key]['curr'] = floatval($src[0][7]);
      $recs[$key]['pid_tail'] = $src[0][1];
      $recs[$key]['track'] = $src[0][3];

      preg_match('@件字號 </font>
    </td>
    <td colspan="4" class="LT1">
       <font size="1"></font>
    </td>
    <td colspan="5" class="L1">
       <font size="1">&nbsp;.*</font><font size=1>&nbsp;(.*)</font>@m', $rec['html'], $m);

      $recs[$key]['buyer'] = $m[1];

      unset($recs[$key]['txt']);
      unset($recs[$key]['html']);
    }
    return $recs;
  }
}

class ExportRecFetcherFedex extends AbstractExportRecFetcher
{
}

class ExportRecFetcherUps extends AbstractExportRecFetcher
{
  protected function getQueryParamTemplate($year, $month, $day)
  {
    $param = parent::getQueryParamTemplate($year, $month, $day);
    $param['company'] = '16632571';
    return $param;
  }
}

class ExportRecFetcherFactory
{
  static public function getFetcher($custId)
  {
    $sql = 'SELECT * FROM {fedfetch_account} WHERE custId = "%s"';
    $account = db_fetch_object(db_query($sql, $custId));

    switch($account->type){
      case 'Fedex':
        $f = new ExportRecFetcherFedex(
          $account->custId, $account->passWord, $account->selectBin);
        break;

      case 'UPS':
        $f = new ExportRecFetcherUps(
          $account->custId, $account->passWord, $account->selectBin);
        break;

      default:
        throw new Exception('Unknow account type of FedFetch.');
    }
    return $f;
  }

  static public function getAllFetchers()
  {
    $sql = 'SELECT custId FROM {fedfetch_account}';
    $rs = db_query($sql);

    $fs = array();
    while($custId = db_result($rs)){
      $fs[] = self::getFetcher($custId);
    }
    return $fs;
  }
}

?>