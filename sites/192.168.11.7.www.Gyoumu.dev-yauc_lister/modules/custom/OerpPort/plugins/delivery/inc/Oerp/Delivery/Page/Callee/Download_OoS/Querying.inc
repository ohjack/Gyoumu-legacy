<?php
namespace Oerp\Delivery\Page\Callee\Download_OoS;

class Querying extends \Oerp\Delivery\Page\Callee\Download_Sent\Abstract_fromView {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function getArch() {
    return file_get_contents(__DIR__ . '/Querying.xml');
  }

  protected function getEleOverrides() {
    return array(
      '#value' => 'Download Querying',
    );
  }
}