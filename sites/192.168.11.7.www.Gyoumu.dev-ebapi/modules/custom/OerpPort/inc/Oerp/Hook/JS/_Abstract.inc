<?php
namespace Oerp\Hook\JS;


abstract class _Abstract {
  const WT_WIZARD = 1;
  const WT_WORKFLOW = 2;
  const WT_ONCHANGE = 4;
  const WT_EXE = 8;

  protected $post;
  protected $send;

  abstract public function getWeight();
  abstract public function applicable();
  abstract public function exe();

  public function __construct() {
    $this->post = $_POST;
    $this->send = json_decode($this->post['send']);
    $this->init();
  }

  protected function init() {
    return;
  }

  protected function succeedWeight($wt) {
    return $wt / 1.5;
  }
}