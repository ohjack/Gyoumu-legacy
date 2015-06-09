<?php
namespace Oerp\Hook\JS;

abstract class Abstract_Exe extends \Oerp\Hook\JS\_Abstract {
  public function getWeight() {
    return self::WT_EXE;
  }

  public function applicable() {
    return is_array($this->send);
  }

  public function exe() {
    $send = $this->send;
    $q = new \Oerp\Query\Basic(array_shift($send));
    $resp = $q->exec($send, true);
    return drupal_json($resp);
  }
}
