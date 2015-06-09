<?php
namespace Oerp\Stock\Operation;

class PackingReturn implements \Gulei\Operation\_Interface {
  protected $rid;
  protected $picking;
  protected $force_pick;

  protected function wiz_exe($args, $action = 'execute') {
    return \Oerp\Query\Wizard::_execute($args, $action);
  }

  public function __construct($rid, $picking, $force_pick = FALSE) {
    $this->rid = $rid;
    $this->picking = $picking;
    $this->force_pick = $force_pick;
  }

  protected function send($wid, $form) {
    $data = $form['datas'];
    $moves = \Oerp\Query::q('stock.move', NULL, array('product_id'), $data['returns']);

    foreach ($moves as $move) {
      preg_match('/\[([^[]*)\]/', $move['product_id'][1], $m);
      $sku = $m[1];
      $data['return' . $move['id']] = $this->picking[$sku];
    }

    $data['invoice_state'] = 'none';

    $send = array(
      $wid,
      array(
        'model' => 'stock.picking',
        'form' => $data,
        'id' => $this->rid,
        'report_type' => 'pdf',
        'ids' => array($this->rid),
      ),
      'return',
    );

    $rs = $this->wiz_exe($send);
    preg_match('/\[(\d*)\]/', $rs['action']['domain'], $m);

    return array(
      'picking_id' => (int)$m[1],
    );
  }

  public function run() {
    $wid = $this->wiz_exe(array('stock.return.picking'), 'create');

    $send = array(
      $wid,
      array(
        'model' => 'stock.picking',
        'form' => array(),
        'id' => $this->rid,
        'ids' => array($this->rid),
      ),
      'init',
    );

    $form = $this->wiz_exe($send);
    $rs = $this->send($wid, $form);

    if ($this->force_pick) {
      $op = new \Oerp\Stock\Operation\ProductReceive($rs['picking_id'], TRUE);
      $rs['picked'] = $op->run();
    }
    return $rs;
  }
}