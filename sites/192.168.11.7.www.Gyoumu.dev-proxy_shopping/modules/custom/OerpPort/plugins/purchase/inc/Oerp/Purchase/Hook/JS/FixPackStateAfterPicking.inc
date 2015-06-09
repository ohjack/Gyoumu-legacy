<?php
namespace Oerp\Purchase\Hook\JS;

class FixPackStateAfterPicking extends \Oerp\Hook\JS\_Abstract {
  protected $wiz;

  public function init() {
    $this->wiz = new \Oerp\Hook\JS\Wizard();
  }

  public function getWeight() {
    return $this->succeedWeight(self::WT_WIZARD);
  }

  public function applicable() {
    return $this->wiz->applicable()
        && $this->send->action == 'split'
        && $this->send->model == 'stock.picking';
  }

  public function exe() {
    $this->wiz->exe();

    $pick = \Oerp\Query::q(
      'stock.picking', NULL, array('purchase_id'), $this->send->rid);

    if ($pick_id = $pick[0]['purchase_id'][0]) {
      $prov = new \Oerp\Purchase\Provider\PoLines($pick_id);
      $recs = $prov->getData();
      $rec = $recs[0];

      if ($rec && $rec['state'] == 'done') {
        $prov = new \Oerp\Purchase\Provider\PoPicking(array($rec['id']));
        $picking = $prov->getData();
        $picking = array_pop($picking);

        $over_received = FALSE;

        foreach ($picking as $sku => $line) {
          if (strpos($sku, '_') !== 0) {
            if ($line['done'] > $rec['order_line'][$sku]['product_qty']) {
              $over_received = TRUE;
              break;
            }
          }
        }

        if ($over_received) {
          $q = new \Oerp\Query('purchase.order', NULL, NULL, $rec['id']);
          $q->write(array('state' => 'except_picking'));
        }
      }
    }
  }
}