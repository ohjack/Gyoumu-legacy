<?php
namespace Gulei\Ftp;

class Client
{
  protected $host;
  protected $username;
  protected $password;
  protected $port;
  private $isLoggedIn = false;
  private $conn_id;

  function __construct($host, $user, $pass, $port = 21)
  {
    $this->host = $host;
    $this->username = $user;
    $this->password = $pass;
    $this->port = $port;
  }

  function __destruct()
  {
    ftp_close($this->conn_id);
  }

  public function isLoggedIn()
  {
    return $this->isLoggedIn;
  }

  public function login()
  {
    if(!$this->isLoggedIn()){
      $this->conn_id = ftp_connect($this->host, $this->port);

      if($this->conn_id){
        $this->isLoggedIn = ftp_login($this->conn_id, $this->username, $this->password);
      }
    }
  }

  public function getList($path)
  {
    return ftp_nlist($this->conn_id, $path);
  }

  public function existFiles($path, $files)
  {
    $exist = $this->getList($path);
    $missing = array_diff($files, $exist);
    return empty($missing);
  }
}