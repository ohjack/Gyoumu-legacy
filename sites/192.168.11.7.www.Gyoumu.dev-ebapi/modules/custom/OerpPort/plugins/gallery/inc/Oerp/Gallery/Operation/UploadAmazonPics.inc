<?php
namespace Oerp\Gallery\Operation;

class UploadAmazonPics implements \Gulei\Operation\_Interface {
  public function run() {
    $up = new \Oerp\Gallery\File\Uploader\Amazon();
    $up->exe();
  }
}