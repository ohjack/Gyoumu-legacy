<?php
namespace Oerp\Purchase\Controller;

class PoWithMove {
  protected $po;
  protected $pick;
  protected $pick_id;
  protected $ts;

  public function __construct($po) {
    list($this->po) = array(
      $po,
    );

    $this->rid = $this->po['id'];
    $this->ts = time();
    $this->init_pick();
  }

  protected function init_pick() {
    $provPack = new \Oerp\Purchase\Provider\PoPicking($this->rid);
    $this->pick = $provPack->getData();
    $this->pick = array_pop($this->pick);

    foreach ($this->pick['_lines'] as $line) {
      if ($line['state'] == 'assigned'
          && $line['picking_id']['type'] == 'in'
          && $pick_id = $line['picking_id']['id'])
      {
        $this->pick_id = $pick_id;
        break;
      }
    }
  }

  protected function getMoveSend($sku, $to_pick, $ref_line) {
    $prod_line = $this->po['order_line'][$sku];

    $is_pick = $to_pick > 0 ? TRUE : FALSE;

    $loc_from = $is_pick ? 7 : 11;
    $loc_to = $is_pick ? 11 : 7;

    $name = sprintf('[%s] %s : %s $%s$',
      $is_pick ? 'PICK' : 'RETURN', $this->po['name'], $sku, $this->ts);

    $pick_note = trim($ref_line['pick_note']);
    $pick_note = $pick_note ? "\n" . $pick_note : $pick_note;

    $note = sprintf('[%s]%s',
      $is_pick ? 'PICK' : 'RETURN', $pick_note);

    return array(
      'picking_id' => $this->pick_id,
      'location_id' => $loc_from,
      'location_dest_id' => $loc_to,
      'product_id' => $prod_line['product_id']['id'],
      'product_uom' => $prod_line['product_uom']['id'],
      'product_qty' => abs($to_pick),
      'name' => $name,
      'date_planned' => $ref_line['pick_date'],
      'date' => date('Y-m-d H:i:s'),
      'note' => $note,
    );
  }

  protected function linkPoMoves($new_moves) {
    $po_send = array();

    foreach ($new_moves as $sku => $new_move) {
      $oid = $this->po['order_line'][$sku]['id'];

      $po_send['order_line'][] = array(
        1, $oid,
        array(
          'move_ids' => array(
            array(
              0, 0, $new_move
            ),
          ),
        ),
      );
    }

    $q = new \Oerp\Query('purchase.order', NULL, NULL, $this->po['id']);
    $q->write($po_send);
  }

  protected function updatePoMoveStates($state) {
    $states = array('draft', 'confirmed', 'assigned', 'done');

    if (!in_array($state, $states, TRUE)) {
      return FALSE;
    }

    $q = new \Oerp\Query(
      'stock.move',
      array(array('name', 'like', '$' . $this->ts . '$')),
      array('id')
    );

    $new_move_ids = $q->raw()->collect('id');

    if (!$new_move_ids) {
      return FALSE;
    }

    $ops = array(
      'draft' => new \Oerp\Stock\Operation\ConfirmOrderline($new_move_ids),
      'confirmed' => new \Oerp\Stock\Operation\ConfirmAvailability($new_move_ids),
      'assigned' => new \Oerp\Stock\Operation\MarkMoveAsDone($new_move_ids),
    );

    foreach ($ops as $stop_state => $op) {
      if ($stop_state == $state) {
        return TRUE;
      }
      $op->run();
    }
    return TRUE;
  }

  protected function chkPick($line, $sku, $state) {
    return $line['product_id']['default_code'] == $sku
        && $line['state'] == $state
        && $line['picking_id']['type'] == 'in';
  }

  protected function setMoveQty($mov_id, $qty) {
    $q = new \Oerp\Query('stock.move', NULL, NULL, $mov_id);
    $q->write(array('product_qty' => $qty));
  }

  protected function minusCascade($sku, $type, $qty) {
    if ($qty <= 0) {
      return;
    }

    foreach ($this->pick['_lines'] as &$pk_line) {
      if ($this->chkPick($pk_line, $sku, $type)) {
        $line_qty = $pk_line['product_qty'];

        if ($line_qty < 1) {
          continue;
        }

        $new_qty = $line_qty - $qty;
        $new_qty = $new_qty < 0 ? 0 : $new_qty;

        $this->setMoveQty($pk_line['id'], $new_qty);
        $pk_line['product_qty'] = $new_qty;
        $qty -= $line_qty - $new_qty;
      }

      if ($qty < 1) {
        break;
      }
    }
  }

  protected function plusCascade($sku, $type, $qty) {
    if ($qty <= 0 ) {
      return;
    }

    foreach ($this->pick['_lines'] as &$pk_line) {
      if ($this->chkPick($pk_line, $sku, $type)) {
        $line_qty = $pk_line['product_qty'];

        $new_qty = $line_qty + $qty;
        $this->setMoveQty($pk_line['id'], $new_qty);
        $pk_line['product_qty'] = $new_qty;
        return;
      }
    }
  }

  protected function fixVirtualStock($to_picks) {
    foreach ($to_picks as $sku => $change) {
      if ($change > 0) {
        $this->minusCascade($sku, 'assigned', $change);
      }
      else if ($change < 0) {
        $abs_change = (int)abs($change);
        $qty = $this->po['order_line'][$sku]['product_qty'];
        $done = $this->pick[$sku]['done'];

        if ($done > $qty) {
          $abs_change -= ($done - $qty);
        }

        $this->plusCascade($sku, 'assigned', $abs_change);
      }
    }
  }

  public function createPoMoves($ref_lines, $state) {
    $new_moves = array();
    $to_picks = array();

    foreach ($ref_lines as $sku => $ref_line) {
      $to_pick = $ref_line['pick_qty'];

      if ($to_pick != 0) {
        $new_moves[$sku] = $this->getMoveSend($sku, $to_pick, $ref_line);
        $to_picks[$sku] = $to_pick;
      }
    }

    $this->linkPoMoves($new_moves);
    $this->updatePoMoveStates($state);
    $this->fixVirtualStock($to_picks);
  }

  public function postfix() {
    $hooks = array(
      new \Oerp\Purchase\Hook\Form\FixPoState(),
    );

    foreach ($hooks as $hook) {
      $param = array('rid' => $this->rid);
      $hook->execute($param);
    }
  }
}