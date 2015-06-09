<?php
namespace Oerp\Stock\Operation;

class ConfirmOrderline implements \Gulei\Operation\_Interface {
  protected $ids;

  public function __construct($ids) {
    $this->ids = is_array($ids) ? $ids : array($ids);
  }

  public function run() {
    $args = array(
      'stock.move',
      'action_confirm',
      $this->ids,
    );
    return \Oerp\Query::_execute($args);
  }
}
