<?php
namespace Checkimage\Query;

class PictureMaster extends \Gulei\File\Finder\_Abstract
{
  const host = 'picturemaster.info';
  static protected $client;
  private $path;

  function __construct($path)
  {
    if (!self::$client) {
      self::$client = new \Gulei\Ftp\Client(self::host, 'pictu39', '2xaugadr');
      self::$client->login();
    }
    $this->path = $path;
  }

  public function getList()
  {
    return self::$client->getList($this->path);
  }
}