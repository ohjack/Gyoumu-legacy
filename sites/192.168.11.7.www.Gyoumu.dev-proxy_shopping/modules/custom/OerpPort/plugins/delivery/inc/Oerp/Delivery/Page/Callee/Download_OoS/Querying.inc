<?php
namespace Oerp\Delivery\Page\Callee\Download_OoS;

class Querying extends \Oerp\Delivery\Page\Callee\Download_Sent\Abstract_fromView {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function getQueryParam() {
    $param = parent::getQueryParam();
    $param['copy_method'] = 'duplicate';
    return $param;
  }

  protected function getArch() {
    return $this->FormPage->getArch();
  }

  protected function getEleOverrides() {
    return array(
      '#value' => 'Download Querying',
    );
  }
}