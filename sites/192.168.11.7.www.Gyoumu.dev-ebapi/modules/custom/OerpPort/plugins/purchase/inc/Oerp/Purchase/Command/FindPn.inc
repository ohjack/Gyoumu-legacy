<?php
namespace Oerp\Purchase\Command;

class FindPn extends \Gulei\Command\WithReference\WithTable {
  public function execute($RefVar = NULL) {
    static $supps = NULL;
    static $recs = NULL;

    if ($supps === NULL) {
      list($recs) = $this->ref->vals('recs');

      $ids = \Gulei\_Array::lambda($recs, 'collect', 'order_line.product_id.id');
      $ids = array_filter($ids);
      $ids = array_values($ids);

      if (!$ids) {
        $supps = array();
      }
      else {
        $supp_infos = \Oerp\Query::q(
          'product.supplierinfo',
          array(array('product_id', 'in', $ids)),
          array('name', 'product_id', 'product_code')
        );

        foreach ($supp_infos as $supp_info) {
          $pid = $supp_info['product_id'][0];
          $sup_id = $supp_info['name'][0];
          $supps[$pid][$sup_id] = $supp_info['product_code'];
        }
      }
    }

    list($rec, $ind) = $this->ref->vals('rec', 'ind');
    $pid = $rec['order_line.product_id.id'];
    $sup_id = $rec['partner_id.id'];

    if ($sup_id === '_DITTO_') {
      for ($i = $ind; $ind >= 0; $i--) {
        $sup_id = $recs[$i]['partner_id.id'];

        if ($sup_id != '_DITTO_') {
          break;
        }
      }
    }
    return $supps[$pid][$sup_id];
  }
}