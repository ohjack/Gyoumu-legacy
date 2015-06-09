<?php
namespace Oerp\Purchase\Command;

class SafetyStockInForm extends \Gulei\Command\WithReference\WithTable {
  public function execute($RefVar = NULL) {
    static $levels = NULL;

    if ($levels === NULL) {
      list($recs) = $this->ref->vals('recs');
      $ids = \Gulei\_Array::lambda($recs, 'collect', 'product_id.id');
      $ids = array_filter($ids);

      if (!$ids) {
        $levels = array();
      }
      else {
        $prov = new \Oerp\Stock\Provider\SafetyStock($ids);
        $levels = $prov->getData();
      }
    }

    list($rec) = $this->ref->vals('rec');
    $level = $levels[$rec['product_id.id']]['sum'];
    return $level;
  }
}