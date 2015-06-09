<?php
namespace Oerp\Query;

class Operation
{
  private $act;
  private $mod;
  private $cri;
  private $ids;
  private $send;

  public function __construct($act, $mod, $cri, $ids, $send)
  {
    $this->act = $act;
    $this->mod = $mod;
    $this->cri = $cri;
    $this->ids = $ids;
    $this->send = $send;
  }

  public function run()
  {
    $q = new \Oerp\Query\Basic($this->mod, $this->cri, null, $this->ids);
    $act = $this->act;

    return $q->$act($this->send);
  }
}