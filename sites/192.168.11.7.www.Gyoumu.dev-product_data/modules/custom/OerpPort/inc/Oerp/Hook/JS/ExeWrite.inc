<?php
namespace Oerp\Hook\JS;


class ExeWrite extends \Oerp\Hook\JS\Abstract_Exe {
  public function getWeight() {
    return $this->succeedWeight(parent::getWeight());
  }

  public function applicable() {
    return $this->send[1] == 'write';
  }

  public function exe() {
    $send = $this->send;
    $rec = (array)$send[3];

    if (isset($rec['#value'])) {
      $send[3] = \Oerp\Util\Helper::renderNestedInput($rec['#value']);
    }

    $this->send = $send;
    parent::exe();
  }
}