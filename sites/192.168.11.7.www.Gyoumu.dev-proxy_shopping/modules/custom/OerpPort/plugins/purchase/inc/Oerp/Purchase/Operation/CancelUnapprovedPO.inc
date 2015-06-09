<?php
namespace Oerp\Purchase\Operation;

class CancelUnapprovedPO implements \Gulei\Operation\_Interface {
  public function run() {
    $prov = new \Oerp\Purchase\Provider\UnapprovedOrder();
    $ids = $prov->getData()->collect('id');
    $q = new \Oerp\Query('purchase.order');

    foreach ($ids as $id) {
      $q->exec_workflow(array('purchase_cancel', $id));
    }
  }
}