<?php
namespace Gulei\Ftp;

class ClientMass extends \Gulei\Ftp\Client {
  public function getList($path) {
    $Ssh = new \Gulei\SSH\Client(
      $this->host, $this->username, $this->password);

    $src = $Ssh->exe(sprintf('ls -1 %s', $path));
    return explode("\n", $src);
  }
}