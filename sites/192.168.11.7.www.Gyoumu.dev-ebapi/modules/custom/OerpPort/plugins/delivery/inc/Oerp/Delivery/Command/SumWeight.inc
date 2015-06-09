<?php
namespace Oerp\Delivery\Command;

class SumWeight extends \Gulei\Command\WithReference\WithTable {
  static protected $weight_map = NULL;

  public function execute($ref_val = null) {
    if (self::$weight_map === NULL) {
      $recs = $this->ref->vals('recs');
      $recs = $recs[0];

      $prod_ids = array();
      $prod_ids_src = \Gulei\_Array::lambda($recs, 'collect', 'move_lines.product_id.id');

      foreach($prod_ids_src as $prod_id_src) {
        $prod_ids = array_merge($prod_ids, explode("\n", $prod_id_src));
      }

      $prod_ids = array_map('trim', $prod_ids);
      $prod_ids = array_unique($prod_ids);
      $prod_ids = array_values($prod_ids);

      if (!$prod_ids) {
        self::$weight_map = array();
      }
      else {
        $packs = \Oerp\Query::q(
          'product.packaging',
          array(
            array('product_id', 'in', $prod_ids)
          ),
          array('product_id', 'weight')
        )->promote('product_id', 0);

        if ($packs) {
          self::$weight_map = $packs;
        }
        else {
          self::$weight_map = array();
        }
      }
    }

    $rec = $this->ref->vals('rec');
    $pids = explode("\n", $rec[0]['move_lines.product_id.id']);
    $total_weight = 0;

    foreach ($pids as $pid) {
      $pid = trim($pid);
      $weight = self::$weight_map[$pid]['weight'];

      if ($weight === NULL) {
       $total_weight = '*MISSING*';
        break;
      }
      $total_weight += $weight;
    }
    return $total_weight;
  }
}