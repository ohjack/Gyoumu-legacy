<?php
namespace Oerp\Purchase\Command;

class PoPickingStatus extends \Gulei\Command\WithReference\WithTable {
  public function execute($RefVar = NULL) {
    static $last_oname = NULL;
    static $pickings = NULL;

    if ($pickings === NULL) {
      $ids = \Gulei\_Array::lambda($this->ref->val('recs'), 'collect', 'id');
      $ids = array_values(array_filter($ids));

      $prov = new \Oerp\Purchase\Provider\PoPicking($ids);
      $pickings = $prov->getData();
    }

    list($rec) = $this->ref->vals('rec');

    if ($rec && $rec['name'] != '_DITTO_') {
      $last_oname = $rec['name'];
    }

    $sku = $rec['order_line.product_id.code'];
    $pick = $pickings[$last_oname];

    $qty = $pick[$sku]['done'];
    $qty = $qty ? $qty : 0;
    return '/ ' . $qty;
  }
}