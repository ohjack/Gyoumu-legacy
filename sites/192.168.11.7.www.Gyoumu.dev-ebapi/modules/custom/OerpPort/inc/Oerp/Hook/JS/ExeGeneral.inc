<?php
namespace Oerp\Hook\JS;

class ExeGeneral extends \Oerp\Hook\JS\_Abstract {
  public function getWeight() {
    return self::WT_EXE;
  }

  public function applicable() {
    return TRUE;
  }

  public function exe() {
    $send = $this->send;
    $resp = \Oerp\Query\Basic::_execute(
      array(
        $send[0],
        $send[1],
        array((int)$send[2])
      )
    );
    return drupal_json($resp);
  }
}