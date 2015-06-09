<?php
namespace Yauc\Driver;

abstract class _Abstract
{
  protected $sel_id;
  protected $pwd;
  protected $driver;

  abstract protected function createDriver();

  function __construct($sel_id){
    $this->sel_id = $sel_id;

    $sql = 'SELECT password FROM {yaucapi_sellers} WHERE seller_id="%s"';
    $this->pwd = db_result(db_query($sql, $sel_id));
  }

  protected function initDriver(){
    if(!$this->driver){
      $this->driver = $this->createDriver();
      $this->driver->init();
    }
  }

  public function getDriver()
  {
    return $this->driver;
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

  protected function prepareDriver($action)
  {
    $this->initDriver();

    if(!$this->checkDriverReady(false, true)){
      watchdog(
        'GuYauc', 'Account @seller is not ready. @action terminated.',
        array(
             '@seller' => $this->sel_id,
             '@action' => $action
        ), WATCHDOG_ERROR);

      return false;
    }
    return true;
  }
}