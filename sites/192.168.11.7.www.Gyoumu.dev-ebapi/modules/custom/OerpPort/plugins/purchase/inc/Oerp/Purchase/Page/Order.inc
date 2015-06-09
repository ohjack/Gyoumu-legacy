<?php
namespace Oerp\Purchase\Page;

class Order extends \Oerp\Page\Abstract_FormView {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function getArch() {
    return file_get_contents(__DIR__ . '/Order.xml');
  }

  protected function getParam() {
    $get = $_GET;

    return array(
      'model' => $get['model'],
      'vid' => $get['vid'],
      'rid' => $get['rid'],
      'arch' => $this->getArch(),
    );
  }
}