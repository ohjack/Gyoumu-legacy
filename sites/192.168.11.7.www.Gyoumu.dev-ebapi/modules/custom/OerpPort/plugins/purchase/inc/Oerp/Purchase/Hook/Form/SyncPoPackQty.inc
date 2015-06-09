<?php
namespace Oerp\Purchase\Hook\Form;

class SyncPoPackQty extends \Oerp\Purchase\Hook\Form\Abstract_PoController {
  protected function isApplicableState($state) {
    return $state == 'approved' || $state == 'done';
  }

  protected function _exe($param) {
    $picking = $this->getPicking();
    $search_type = $this->getSearchType();
    $state = $this->rec['state'];

    if (!$this->isApplicableState($state)) {
      return FALSE;
    }

    foreach ($this->rec['order_line'] as $line) {
      $sku = $line['product_id']['default_code'];

      if (!$this_pick = $picking[$sku]) {
        continue;
      }

      $qty_op = $line['product_qty'];
      $qty_pa = $this_pick['assigned'] + $this_pick['done'];

      $diff = (int)($qty_op - $qty_pa);

      if ($diff <> 0) {
        foreach ($picking['_lines'] as &$move) {
          if ($move['product_id']['default_code'] == $sku
              && $move['state'] === $search_type) {

            $qty_line = $move['product_qty'];

            $q = new \Oerp\Query('stock.move', NULL, NULL, $move['id']);

            if ($qty_line + $diff >= 0) {
              $new_qty = $qty_line + $diff;
              $q->write(array('product_qty' => $new_qty));

              $move['product_qty'] = $new_qty;
              break;
            }
            else {
              $q->write(array('product_qty' => 0));

              $move['product_qty'] = 0;
              $diff = $diff + $qty_line;
              continue;
            }
          }
        }
      }
    }
  }
}