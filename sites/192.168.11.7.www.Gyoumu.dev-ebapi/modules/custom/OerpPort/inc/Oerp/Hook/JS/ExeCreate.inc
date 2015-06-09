<?php
namespace Oerp\Hook\JS;


class ExeCreate extends \Oerp\Hook\JS\Abstract_Exe {
  public function getWeight() {
    return $this->succeedWeight(parent::getWeight());
  }

  public function applicable() {
    return $this->send[1] == 'create';
  }

  public function exe() {
    $send = $this->send;
    $rec = (array)$send[2];

    if (isset($rec['#value'])) {
      $send[2] = \Oerp\Util\Helper::renderNestedInput($rec['#value']);
    }

    $this->send = $send;
    parent::exe();
  }
}