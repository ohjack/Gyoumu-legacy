<?php
namespace Gulei\Provider;

abstract class _Abstract implements \Gulei\Provider\_Interface {
  protected $param = array();

  public function __construct($param = array())
  {
    $this->param = $param;
    $this->init();
  }

  protected function init() { }

  public function overrideParam($param) {
    $this->param = array_merge($this->param, $param);
  }
}