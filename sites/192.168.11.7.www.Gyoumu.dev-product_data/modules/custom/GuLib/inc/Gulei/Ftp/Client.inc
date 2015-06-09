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

  public function getDirs($path) {
    $list = $this->getList($path);

    foreach ($list as $ind => $each) {
      if ($each == '.' || $each == '..') {
        unset($list[$ind]);
      }

      if (ftp_size($this->conn_id, $path . '/' . $each) !== -1) {
        unset($list[$ind]);
      }
    }
    return $list;
  }

  public function existFiles($path, $files)
  {
    $exist = $this->getList($path);
    $missing = array_diff($files, $exist);
    return empty($missing);
  }

  public function existDir($dir)
  {
    return is_dir(sprintf('ftp://%s:%s@%s/%s',
      $this->username, $this->password, $this->host, $dir
    ));
  }

  public function delete($path) {
    return ftp_delete($this->conn_id, $path);
  }

  public function mkdir($dir)
  {
    return ftp_mkdir($this->conn_id, $dir);
  }

  public function upload($local, $remote, $mode = \FTP_ASCII, $startpos = 0)
  {
    $rs = ftp_put($this->conn_id, $remote, $local, $mode, $startpos);
    ftp_chmod($this->conn_id, 0666, $remote);
    return $rs;
  }
}