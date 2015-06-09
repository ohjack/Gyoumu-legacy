<?php
namespace Oerp\Stock\Operation;

class MarkMoveAsDone implements \Gulei\Operation\_Interface {
  protected $ids;

  public function __construct($ids) {
    $this->ids = is_array($ids) ? $ids : array($ids);
  }

  public function run() {
    $q = new \Oerp\Query('stock.move', NULL, NULL, $this->ids);
    return $q->write(array('state' => 'done'));
  }
}
