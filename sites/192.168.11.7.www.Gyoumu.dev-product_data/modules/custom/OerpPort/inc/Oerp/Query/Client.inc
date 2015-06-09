<?php
namespace Oerp\Query;
use Oerp as O;

class Client
{
  private $server;
  private $database;
  private $username;
  private $password;
  private $uid = null;

  function __construct($srv = null, $db = null, $uname = null, $pass = null)
  {
    gulib_inc('xmlrpc.inc', 'lib');

    if ($srv && $db && $uname && $pass) {
      $this->connect($srv, $db, $uname, $pass);
    }
  }

  public function connect($srv, $db, $uname, $pass)
  {
    $this->server = $srv;
    $this->database = $db;
    $this->username = $uname;
    $this->password = $pass;

    $sock = new \xmlrpc_client($this->server . '/xmlrpc/common');
    $msg = new \xmlrpcmsg('login');
    $msg->addParam(new \xmlrpcval($this->database));
    $msg->addParam(new \xmlrpcval($this->username));
    $msg->addParam(new \xmlrpcval($this->password));
    $resp = $sock->send($msg);

    if ($resp->faultCode()) {
      $this->printError($resp->faultString());
    }
    else {
      $this->uid = $resp->value()->scalarVal();

      if (!$this->uid) {
        die(sprintf(
          "Bad login or password from '%s' using database '%s'",
          $this->username, $this->database));
      }
    }
  }

  public function execute($args, $action = 'execute', $reset = false, $service = 'object')
  {
    if ($this->uid === null) {
      die('Error: Connection have not established');
      return null;
    }

    //encoding settings for reading-related actions
    global $xmlrpc_internalencoding;

    if ($service == 'object') {
      $pat = '/(create|write|search)/';
      if (preg_match($pat, $args[1])) {
        $xmlrpc_internalencoding = 'ISO-8859-1';
      } else {
        $xmlrpc_internalencoding = 'UTF-8';
      }

      //pad empty arguments
      switch ($args[1]) {
        case 'search':
          $args = array_pad($args, 4, array());
          break;
      }
    }

    $msg = new \xmlrpcmsg($action);
    $msg->addParam(new \xmlrpcval($this->database));
    $msg->addParam(new \xmlrpcval($this->uid, "int"));
    $msg->addParam(new \xmlrpcval($this->password));

    foreach ($args as $arg) {
      $msg->addParam(php_xmlrpc_encode($arg));
    }

    $debug = false;
    if ($debug) {
      _gecho($msg);
    }

    //  only cache search & read
    if ($args[1] != 'search' && $args[1] != 'read') {
      $reset = true;
    }

    if ($reset) {
      return $this->send($msg, $service);
    }

    //  cache using Drupal native API
    $cache_id = 'query:' . md5($msg->serialize());
    $cache = cache_get($cache_id, 'cache_oerpport');
    $time = $_SERVER['REQUEST_TIME'];

    if ($cache && $cache->expire > $time) {
      $resp = $cache->data;
    }
    else{
      $resp = $this->send($msg, $service);
      $expire = $this->getExpireTime($args);

      if ($expire) {
        cache_set($cache_id, $resp, 'cache_oerpport', $expire);
      }
    }

    return $resp;
  }

  protected function send($msg, $service)
  {
    $sock = new \xmlrpc_client($this->server . '/xmlrpc/' . $service);
    $resp = $sock->send($msg);

    if ($resp->faultCode()) {
      $this->printError($resp->faultString());
    }

    return $resp->value();
  }

  public function exe($args)
  {
    return php_xmlrpc_decode($this->execute($args));
  }

  protected function getExpireTime($args)
  {
    $time = $_SERVER['REQUEST_TIME'];

    switch ($args[0]) {
      case 'ir.ui.view':
      case 'ir.model.fields':
//      expire in 1 day
        return $time + 3600 * 24;

      default:
        return $time + 1;
    }
  }

  protected function printError($msg)
  {
    echo '<pre>' . $msg;
    throw new \Exception('Error in OerpClient');
  }
}
