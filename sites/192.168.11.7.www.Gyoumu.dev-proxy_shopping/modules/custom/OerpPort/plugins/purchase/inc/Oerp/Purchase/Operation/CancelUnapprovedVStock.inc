<?php
namespace Oerp\Purchase\Operation;

class CancelUnapprovedVStock implements \Gulei\Operation\_Interface {
  public function run() {
    $prov = new \Oerp\Purchase\Provider\DraftOrders();
    $ids = $prov->getData()->collect('id');

    $prov = new \Oerp\Purchase\Provider\PoPicking($ids);
    $pickings = $prov->getData();

    $mov_ids = array();
    foreach ($pickings as $po) {
      foreach ($po['_lines'] as $mov) {
        $mov_ids[] = $mov['id'];
      }
    }

    $q = new \Oerp\Query('stock.move', NULL, NULL, $mov_ids);
    $q->write(
      array(
        'product_qty' => 0
      )
    );
  }
}