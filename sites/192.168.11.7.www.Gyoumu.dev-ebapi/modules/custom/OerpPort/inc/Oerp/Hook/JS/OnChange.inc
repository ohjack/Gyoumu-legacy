<?php
namespace Oerp\Hook\JS;

class OnChange extends \Oerp\Hook\JS\_Abstract {
  public function getWeight() {
    return self::WT_ONCHANGE;
  }

  public function applicable() {
    if ($this->post['onchange']){
      return TRUE;
    }
    return FALSE;
  }

  public function exe() {
    $resp = \Oerp\Query\Basic::_execute(
      $this->send
    );
    return drupal_json($resp);
  }
}