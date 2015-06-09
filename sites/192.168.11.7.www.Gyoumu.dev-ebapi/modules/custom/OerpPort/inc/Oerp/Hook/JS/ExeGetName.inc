<?php
namespace Oerp\Hook\JS;

class ExeGetName extends \Oerp\Hook\JS\Abstract_Exe {
  public function getWeight() {
    return $this->succeedWeight(parent::getWeight());
  }

  public function applicable() {
    return $this->send[1] == 'name_get';
  }

  public function exe() {
    parent::exe();
  }
}