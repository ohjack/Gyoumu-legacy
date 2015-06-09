<?php
namespace Oerp\Purchase\Command;

class PoPickingStatusInForm extends \Gulei\Command\WithReference\WithTable {
  public function execute($RefVar = NULL) {
    static $pickings = NULL;
    list($rec) = $this->ref->vals('rec');

    if ($pickings === NULL) {
      if (!$rec['id']) {
        $pickings = array();
      }
      else {
        $remote_rec = \Oerp\Query::q(
          'purchase.order.line', NULL, array('order_id'), $rec['id']);

        $remote_rec = $remote_rec[0];
        $prov = new \Oerp\Purchase\Provider\PoPicking(
          array($remote_rec['order_id'][0]));

        $pickings = $prov->getData();

        if ($pickings) {
          $pickings = array_pop($pickings);
        }
        else {
          $pickings = array();
        }
      }
    }

    $sku = $rec['product_id.default_code'];
    $pick = $pickings[$sku]['done'];
    $pick = $pick ? $pick : 0;
    return '/ ' . $pick;
  }
}