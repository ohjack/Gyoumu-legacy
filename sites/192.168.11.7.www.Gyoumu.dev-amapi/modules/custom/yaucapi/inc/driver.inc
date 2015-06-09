<?php

require_once 'PHPUnit/Autoload.php';

define('YAUC_FLAG_SUCCESS', 1);
define('YAUC_FLAG_END', 2);
define('YAUC_FLAG_RELISTED', 3);

class DummySeleniumTest extends PHPUnit_Extensions_SeleniumTestCase
{
    protected function setUp() {}
}

abstract class AbstractYaucDriver extends PHPUnit_Extensions_SeleniumTestCase_Driver
{
  private $username;
  private $password;
  private $cid_session;
  private $login_page = 'https://login.yahoo.co.jp/config/login?.lg=jp&.intl=jp&.src=auc&.done=http://auctions.yahoo.co.jp/';

  const URL_INDEX = 'http://auctions.yahoo.co.jp/';

  function __construct($username, $password)
  {
    parent::__construct();

    $this->username = $username;
    $this->password = $password;
    $this->cid_session = 'SeleniumRcSessionId_' . $username;

    $browser = array(
      'name' => 'YaucDriver',
      'browser' => '*firefox',
//      'host' => '192.168.11.24',
      'host' => '192.168.11.12',
      'port' => 4444,
    );

    $this->setName($browser['name']);
    $this->setHost($browser['host']);
    $this->setPort($browser['port']);
    $this->setBrowser($browser['browser']);
    $this->setBrowserUrl('http://auctions.yahoo.co.jp/');
    $this->setTestCase(new DummySeleniumTest());
  }

  public function getSessionId(){
    return $this->sessionId;
  }

  public function setSessionId($sid){
    $this->sessionId = $sid;
  }

  public function getHttpTimeout(){
    return $this->httpTimeout;
  }

  public function init(){
    $sessionId = cache_get($this->cid_session);

    if($sessionId){
      $this->setSessionId($sessionId->data);
    }

    $this->start();

    if(!$this->checkRcConnection()){
      $this->stop();
      $this->setSessionId(null);
      $this->start();
    }
    cache_set($this->cid_session, $this->getSessionId());
  }

  public function checkRcConnection()
  {
    $httpTimeout = $this->getHttpTimeout();
    $this->setHttpTimeout(1);
    $isConnected = $this->doCommand('getLocation') !== false;
    $this->setHttpTimeout($httpTimeout);
    return $isConnected;
  }

  public function getSource($locator)
  {
    return $this->getString('getText', array($locator));
  }

  public function gotoIndex()
  {
    $loc = $this->getLocation();
    if($loc == self::URL_INDEX){
      return true;
    }
    return $this->open(self::URL_INDEX);
  }

  public function getBodySource()
  {
    return $this->getSource('xpath=/html/body');
  }

  public function getUser()
  {
    $this->gotoIndex();
    $resp = $this->getBodySource();
    preg_match('/こんにちは、 (.*) さん/', $resp, $m);
    return $m[1];
  }

  protected function isInCapcha()
  {
    $src = $this->getBodySource();
    return strpos($src, '次の画像に表示されている少し見えにくい文字を入力してください。') !== false
        ? true : false;
  }

  public function login()
  {
    if($this->getUser() !== $this->username){
      $this->open($this->login_page);
      $this->type("id=username", $this->username);
      $this->type("id=passwd", $this->password);
      $this->click("id=.save");
      $this->waitForPageToLoad("30000");

      if($this->isInCapcha()){
        return false;
      }

      if($this->getUser() !== $this->username){
        return false;
      }
    }
    return true;
  }

  protected function processEndAuction()
  {
    if($this->isTextPresent('オークションは既に終了したか、取り消されました。')){
      return true;
    }

    if(!$this->isElementPresent('name=confirm')){
      return false;
    }
    
    $this->doCommand('clickAndWait', array('name=confirm'));
    return $this->isTextPresent('オークションは終了しましたが、落札者はいません。');
  }

  public function endAuctionByLink($link)
  {
    $parsed = parse_url($link);
    $path_token = explode('/', $parsed['path']);

    $goto = sprintf(
      '%s://%s/jp/show/closeauction?aID=%s',
      $parsed['scheme'], $parsed['host'], $path_token[count($path_token) - 1]
    );

    $this->open($goto);
    return $this->processEndAuction();
  }

  public function endAuctionById($id)
  {
    $this->gotoAuctionById($id);
    $link = $this->getString('getLocation', array());
    $this->endAuctionByLink($link);
  }

  public function relistAuction($link)
  {
    $parsed = parse_url($link);
    $path_token = explode('/', $parsed['path']);

    $goto = sprintf(
      '%s://%s/jp/show/resubmit?aID=%s',
      $parsed['scheme'], $parsed['host'], $path_token[count($path_token) - 1]
    );

    $this->open($goto);

    if($this->isTextPresent('このオークションはすでに再出品されています。')){
      return YAUC_FLAG_RELISTED;
    }

    if(!$this->isElementPresent('id=auc_insertion_ok')){
      return false;
    }

    $this->doCommand('click', array('id=auc_insertion_ok'));
    $this->doCommand('type', array('name=featuredAmount', '10'));

    if($this->isTextPresent('送料全国一律0円（税込み）')){
      $this->doCommand('click', array('auc_shipping_seller'));
    }

    $this->doCommand('clickAndWait', array('id=auc_submit2'));
    $this->doCommand('clickAndWait', array('id=auc_preview_submit'));

    if($this->isTextPresent('以下の商品の出品手続きが完了しました。ご利用ありがとうございました。')){
      return YAUC_FLAG_SUCCESS;
    }
    return false;
  }

  protected function doCommand($command, array $arguments = array())
  {
    if (!ini_get('allow_url_fopen')) {
      watchdog(
        'YaucDriver', 'Could not connect to the Selenium RC server because allow_url_fopen is disabled.',
        array(), WATCHDOG_ERROR
      );
      return false;
    }

    $url = sprintf(
      'http://%s:%s/selenium-server/driver/?cmd=%s',
      $this->host,
      $this->port,
      urlencode($command)
    );

    $numArguments = count($arguments);

    for ($i = 0; $i < $numArguments; $i++) {
      $argNum = strval($i + 1);
      $url .= sprintf('&%s=%s', $argNum, urlencode(trim($arguments[$i])));
    }

    if (isset($this->sessionId)) {
      $url .= sprintf('&%s=%s', 'sessionId', $this->sessionId);
    }

    $this->commands[] = sprintf('%s(%s)', $command, join(', ', $arguments));

    $context = stream_context_create(
      array(
           'http' => array(
             'timeout' => $this->httpTimeout
           )
      )
    );

    $handle = @fopen($url, 'r', FALSE, $context);

    if (!$handle) {
      watchdog(
        'YaucDriver', 'Could not connect to the Selenium RC server.',
        array(), WATCHDOG_ERROR
      );
      return false;
    }

    stream_set_blocking($handle, 1);
    stream_set_timeout($handle, $this->httpTimeout);

    /* Tell the web server that we will not be sending more data
so that it can start processing our request */
    stream_socket_shutdown($handle, STREAM_SHUT_WR);

    $response = stream_get_contents($handle);

    fclose($handle);

    if (!preg_match('/^OK/', $response)) {
      watchdog(
        'YaucDriver', "Response from Selenium RC server for @command.\n@resp.\n",
        array(
             '@command' => $this->commands[count($this->commands) - 1],
             '@resp' => $response
        ),
        WATCHDOG_ERROR
      );

      return false;
    }

    return $response;
  }
}

abstract class AbstractYaucListingDriver extends AbstractYaucDriver
{
  protected function initForm()
  {
    if(!$this->isElementPresent('id=auc_insertion_ok')){
      return false;
    }

    $this->doCommand('click', array('id=auc_insertion_ok'));
  }

  protected function isSubmitSuccess()
  {
    if($this->isTextPresent('以下の商品の出品手続きが完了しました。ご利用ありがとうございました。'))
      return true;

    if($this->isTextPresent('このオークションは、仮出品の扱いとなりました'))
      return true;

    return false;
  }
}

class YaucDriver extends AbstractYaucListingDriver
{
  public function gotoAuctionByLink($link)
  {
    $this->open($link);
  }

  public function gotoAuctionById($id)
  {
    $link = 'http://ord.yahoo.co.jp/o/auction/SIG=125spjm7g/EXP=1311395022;_ylt=A3JvdkpO.yhOGSsBVApBPDh8/*-http%3A//auctions.yahoo.co.jp/jp/show/searchauction';
    $link .= '?searchAuction=' . $id;
    $this->open($link);
  }

  protected function processEndAuction()
  {
    if($this->isTextPresent('オークションは既に終了したか、取り消されました。')){
      return true;
    }

    if(!$this->isElementPresent('name=confirm')){
      return false;
    }
    
    $this->doCommand('clickAndWait', array('name=confirm'));
    return $this->isTextPresent('オークションは終了しましたが、落札者はいません。');
  }

  public function endAuctionByLink($link)
  {
    $parsed = parse_url($link);
    $path_token = explode('/', $parsed['path']);

    $goto = sprintf(
      '%s://%s/jp/show/closeauction?aID=%s',
      $parsed['scheme'], $parsed['host'], $path_token[count($path_token) - 1]
    );

    $this->open($goto);
    return $this->processEndAuction();
  }

  public function endAuctionById($id)
  {
    $this->gotoAuctionById($id);
    $link = $this->getString('getLocation', array());
    $this->endAuctionByLink($link);
  }

  public function relistAuction($link)
  {
    $parsed = parse_url($link);
    $path_token = explode('/', $parsed['path']);

    $goto = sprintf(
      '%s://%s/jp/show/resubmit?aID=%s',
      $parsed['scheme'], $parsed['host'], $path_token[count($path_token) - 1]
    );

    $this->open($goto);

    if($this->isTextPresent('このオークションはすでに再出品されています。')){
      return YAUC_FLAG_RELISTED;
    }

    $this->initForm();
    $this->doCommand('clickAndWait', array('id=auc_submit2'));
    $this->doCommand('clickAndWait', array('id=auc_preview_submit'));

    if($this->isTextPresent('以下の商品の出品手続きが完了しました。ご利用ありがとうございました。')){
      return YAUC_FLAG_SUCCESS;
    }
    return false;
  }
}