<?php
namespace Oerp\Gallery\File\Uploader;

class Rakuten extends \Oerp\Gallery\File\Uploader\_Abstract {
  const UPLOAD_LIMIT = 15;
  static $Client;

  public function __construct() {
    self::$Client = new \Gulei\Ftp\Client('upload.rakuten.ne.jp', 'titosoy', 'MAOpj783');
    self::$Client->login();
  }

  protected function getAccount() {
    return 'rakuten_titosoy';
  }

  protected function getUploadLimit() {
    return self::UPLOAD_LIMIT * 1024 * 1024;
  }

  protected function isAvailable() {
    $subs = self::$Client->getList('cabinet/images');

    foreach($subs as $sub){
      $files = self::$Client->getList($sub);
      if($files){
        return false;
      }
    }
    return true;
  }

  protected function upload($up) {
    static $subs = array();

    preg_match('/..-(\d{5})/', $up, $m);
    $sn = intval($m[1]);
    $sub = ((int)(($sn - 1) / 500) + 1 ) * 500;
    $sub = 'cabinet/images/items' . $sub;

    if(!$subs[$sub]){
      if(!self::$Client->existDir($sub)){
        self::$Client->mkdir($sub);
      }
      $subs[$sub] = true;
    }

    $remote_file = $sub . '/' . strtolower($up);
    $local_file = $this->getLocalFile($up);
    return self::$Client->upload($local_file, $remote_file);
  }

  protected function isOverLimit($up) {
    static $size_sum = 0;
    $size_sum += $this->getLocalFileSize($up);
    if ($size_sum > $this->getUploadLimit()) {
      return TRUE;
    }
    return FALSE;
  }

  protected function isSkip($up) {
    if (preg_match('/.*-0\.jpg$/', $up)) {
      return TRUE;
    }
    return FALSE;
  }
}