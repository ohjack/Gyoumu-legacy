<?php
namespace Oerp\Purchase\Command;

abstract class Abstract_MoveWithOrder extends \Gulei\Command\WithReference\WithTable {
  static protected $moves;

  protected function getMove($sku) {
    if(is_null(self::$moves)){
      list($recs) = $this->ref->vals('recs');

      $prod_ids = \Gulei\_Array::lambda(
        $recs, 'collect', 'move_lines.product_id.id');

      $prod_ids = array_unique($prod_ids);
      $prod_ids = array_filter($prod_ids);
      $prod_ids = array_values($prod_ids);

      $Moves = \Oerp\Query::q(
        'stock.move',
        array(
          array('state', '=', 'assigned'),
          array('product_qty', '>', 0),
          array('product_id', 'in', $prod_ids),
          array('location_id', '=', 7),
          array('location_dest_id', '=', 11),
        ),
        array('purchase_line_id')
      );

      $move_ids = $Moves->collect('id');
      $pur_line_ids = $Moves->collect('purchase_line_id', 0);

      $order_ids = \Oerp\Query::q(
        'purchase.order.line',
        NULL,
        array('order_id'),
        $pur_line_ids
      )->collect('order_id', 0);

      $prov = new \Oerp\Purchase\Provider\PoLines($order_ids);

      $pos = $prov->getData();
      $moves = array();

      foreach ($pos as $po) {
        foreach ($po['order_line'] as $sku => $oline) {
          foreach ($oline['move_ids'] as $move) {
            if ($move['state'] == 'assigned'
                && in_array($move['id'], $move_ids, TRUE)) {

              $moves[$sku][] = array(
                'po_name' => $po['name'],
                'po_id' => $po['id'],
                'date_planned' => $oline['date_planned'],
                'move' => $move,
              );
            }
          }
        }
      }

      $funcSort = create_function('$a,$b', '
        return strcmp($a["date_planned"], $b["date_planned"]);
      ');

      foreach ($moves as &$move) {
        usort($move, $funcSort);
      }

      self::$moves = $moves;
    }

    return self::$moves[$sku][0];
  }

  protected function getSku() {
    list($rec) = $this->ref->vals('rec');
    $root = $this->getRootName();
    return $rec[$root . 'product_id.default_code'];
  }
}