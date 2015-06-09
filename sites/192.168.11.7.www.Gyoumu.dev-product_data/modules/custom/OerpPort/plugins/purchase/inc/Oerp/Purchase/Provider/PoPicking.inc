<?php
namespace Oerp\Purchase\Provider;

class PoPicking extends \Gulei\Provider\_Abstract {
  protected $rec_ids;
  protected $recs;

  public function __construct($rec_ids) {
    $this->rec_ids = $rec_ids;
  }

  protected function getRecs() {
    if (!$this->rec_ids) {
      return FALSE;
    }

    if ($this->recs !== NULL) {
      return $this->recs;
    }

    $arch = <<<XML
<tree>
  <param name="oerp:model">purchase.order</param>

  <field name="state"/>
  <field name="name"/>
  <field name="order_line">
    <field name="product_id">
      <field name="default_code"/>
    </field>
    <field name="product_qty"/>
  </field>
  <field name="picking_ids">
    <field name="move_lines">
      <field name="id"/>
    </field>
  </field>
</tree>
XML;

    $q_param = array(
      'oerp:ids' => $this->rec_ids,
    );

    $flag = array('no-cache' => TRUE);

    $q = new \Oerp\Query\Nested($arch, $q_param);
    return $this->recs = (array)$q->sum($flag);
  }

  protected function getOrderLineIds() {
    $oline_ids = array('_ids' => array());

    foreach ($this->getRecs() as $rec) {
      foreach ($rec['order_line'] as $line) {
        if ($oline_id = $line['id'] ) {
          $oline_ids['line-' . $oline_id] = $rec['name'];
          $oline_ids['_ids'][] = $oline_id;
        }
      }
    }
    return $oline_ids;
  }

  protected function getOrderLines($ids) {
    if (!$ids) {
      return array();
    }

    return \Oerp\Query::q(
      'purchase.order.line',
      NULL,
      array('move_ids', 'move_dest_id'),
      $ids
    );
  }

  protected function getMoveIds($oline_ids) {
    if (!$oline_ids) {
      return array();
    }

    $move_ids = array('_ids' => array());

    foreach ($this->getOrderLines($oline_ids) as $oline) {
      foreach ($oline['move_ids'] as $move_id) {
        $move_ids['move-' . $move_id] = $oline['id'];
        $move_ids['_ids'][] = $move_id;
      }

      if ($move_dest_id = $oline['move_dest_id'][0]) {
        $move_ids['move-' . $move_dest_id] = $oline['id'];
        $move_ids['_ids'][] = $move_dest_id;
      }
    }

    foreach ($this->recs as $rec) {
      foreach ($rec['picking_ids'] as $pick) {
        foreach ($pick['move_lines'] as $move) {
          $move_ids['move-po-' . $move['id']] = $rec['name'];
          $move_ids['_ids'][] = $move['id'];
        }
      }
    }

    $move_ids['_ids'] = array_values(array_unique($move_ids['_ids']));
    return $move_ids;
  }

  protected function getMoves($ids) {
    if (!$ids) {
      return array();
    }

    $arch = <<<XML
<tree>
  <param name="oerp:model">stock.move</param>
    <field name="state"/>
    <field name="date"/>
    <field name="date_planned"/>
    <field name="product_id">
      <field name="default_code"/>
    </field>
    <field name="product_qty"/>
    <field name="picking_id">
      <field name="id"/>
      <field name="name"/>
      <field name="type"/>
    </field>
    <field name="location_id"/>
    <field name="location_dest_id"/>
</tree>
XML;

    $flag = array(
      'no-selection-name' => TRUE,
      'no-cache' => TRUE,
    );

    $Q = new \Oerp\Query\Nested($arch, array('oerp:ids' => $ids));
    return $Q->sum($flag);
  }

  public function getData() {
    if (!$this->rec_ids) {
      return array();
    }

    $oline_ids = $this->getOrderLineIds();

    if (!$oline_ids['_ids']) {
      return array();
    }

    $move_ids = $this->getMoveIds($oline_ids['_ids']);
    $move_lines = $this->getMoves($move_ids['_ids']);

    $pickings = array();

    foreach ($move_lines as $line) {
      $sku = $line['product_id']['default_code'];
      $mv_id = $line['id'];

      $key = $move_ids['move-po-' . $mv_id];

      if (!$key) {
        $line_id = $move_ids['move-' . $line['id']];
        $key = $oline_ids['line-' . $line_id];
      }

      if (!$pickings[$key]['_lines']) {
        $pickings[$key]['_lines'] = array();
      }

      if ($line['location_id'] == 'Stock'
          && $line['location_dest_id'] != 'Stock') {

        $line['product_qty'] *= -1;
      }

      $pickings[$key][$sku][$line['state']] += $line['product_qty'];
      $pickings[$key]['_lines'][] = $line;
    }

    if (!$pickings) {
      return $pickings = array();
    }
    return $pickings;
  }

  public function count() {
    return FALSE;
  }

  public function getReference() {
    return array();
  }
}