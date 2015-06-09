<?php
namespace Yauc\Driver\Manager;

abstract class _Abstract
{
  protected $sel_id;
  protected $pwd;
  protected $driver;
  protected $msgs = array();

  abstract protected function createDriver();
  abstract protected function getClassname();

  function __construct($sel_id){
    $this->sel_id = $sel_id;

    $sql = 'SELECT password FROM {yaucapi_sellers} WHERE seller_id="%s"';
    $this->pwd = db_result(db_query($sql, $sel_id));
  }

  public function getSeller()
  {
    return $this->sel_id;
  }

  protected function initDriver(){
    if(!$this->driver){
      $this->driver = $this->createDriver();
      $this->driver->init();
    }
  }

  protected function isLoggedIn(){
    return $this->driver->getUser() === $this->sel_id;
  }

  protected function printDriverNotReadyMessage($msg = true){
    if ($msg) {
      drupal_set_message(
        'Account ' . $this->sel_id . ' is not logged in. Please manually login at the Selenium server.');
    }
  }

  public function checkDriverReady($msg, $reset = false)
  {
    $cid = 'GuYauc:checkDriverReady_' . $this->sel_id;

    if(!$reset){
      $isReady = cache_get($cid)->data;

      if(!$isReady){
        $this->printDriverNotReadyMessage($msg);
      }
      return $isReady;
    }

    $this->initDriver();

    if (!$this->isLoggedIn()) {
      $this->driver->login();

      if (!$this->isLoggedIn()) {
        $this->printDriverNotReadyMessage($msg);
        cache_set($cid, false);
        return false;
      }
      else{
        watchdog('GuYauc', 'Login @seller automatically.', array('@seller' => $this->sel_id));
      }
    }
    cache_set($cid, true);
    return true;
  }

  public function getDriver()
  {
    $this->initDriver();

    if(!$this->checkDriverReady(false, true)){
      watchdog(
        'GuYauc', 'Account @seller is not ready. @action terminated.',
        array(
             '@seller' => $this->sel_id,
             '@action' => $this->getClassname(),
        ), WATCHDOG_ERROR);

      return false;
    }
    return $this->driver;
  }

  private function logStart($Cmd)
  {
    $this->msgs[] = sprintf(
      'Run: %s', get_class($Cmd)
    );
  }

  private function logResponse($Cmd, $resp)
  {
    $this->msgs[] = sprintf(
      ' (%s) %s : %s',
      $resp['success'] ? 'Success' : 'Failed', get_class($Cmd), $resp['msg']
    );
  }

  public function getMessage()
  {
    return $this->msgs;
  }

  public function apply($Cmd)
  {
    $refVar = new \Gulei\Reference\Basic();
    $refVar->add(array(
      'mgr' => $this,
      'drv' => $this->getDriver()
    ));

    $this->logStart($Cmd);
    $resp = $Cmd->execute($refVar);
    $this->logResponse($Cmd, $resp);
    return $resp['success'];
  }
}