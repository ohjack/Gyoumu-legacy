<?php
namespace Gulei\SSH;

class Client {
  protected $conn;

  public function __construct($host, $username, $password) {
    $this->conn = $this->connect($host, $username, $password);
  }

  protected function connect($host, $username, $password) {
    if (!($con = ssh2_connect($host, 22))) {
      return FALSE;
    }

    if (!ssh2_auth_password($con, $username, $password)) {
      return FALSE;
    }
    return $con;
  }

  public function exe($cmd) {
    if (!($stream = ssh2_exec($this->conn, $cmd))) {
      return FALSE;
    }
    else {
      stream_set_blocking($stream, TRUE);
      $data = '';

      while ($buf = fread($stream, 4096)) {
        $data .= $buf;
      }
      fclose($stream);
    }
    return $data;
  }

  public function __destruct() {
    $this->exe('exit;');
  }
}
