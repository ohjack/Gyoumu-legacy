<?php
namespace Oerp\Stock\Operation;

class ProductReceive implements \Gulei\Operation\_Interface {
  protected $rid;
  protected $picking;

  protected function wiz_exe($args, $action = 'execute') {
    return \Oerp\Query\Wizard::_execute($args, $action);
  }

  public function __construct($rid, $picking) {
    $this->rid = $rid;

    if ($picking === TRUE) {
      $arch = <<<XML
<tree>
  <param name="oerp:model">stock.picking</param>

  <field name="move_lines">
    <field name="product_id">
      <field name="default_code"/>
    </field>
    <field name="product_qty"/>
  </field>
</tree>
XML;

      $q = new \Oerp\Query\Nested($arch, array('oerp:ids' => $rid));
      $lines = $q->sum();
      $picking = array();

      foreach ($lines[0]['move_lines'] as $line) {
        $sku = $line['product_id']['default_code'];
        $qty = $line['product_qty'];
        $picking[$sku] = $qty;
      }
    }

    $this->picking = $picking;
  }

  protected function send($wid, $form) {
    $data = $form['datas'];

    foreach ($form['fields'] as $fld_name => $field) {
      foreach ($this->picking as $sku => $qty) {
        $pat = '/^' . $sku . ' /';
        if (preg_match($pat, $field['string'])) {
          $data[$fld_name] = (float)$qty;
        }
      }

      if (!isset($data[$fld_name])) {
        $data[$fld_name] = FALSE;
      }
    }

    $send = array(
      $wid,
      array(
        'model' => 'stock.picking',
        'form' => $data,
        'id' => $this->rid,
        'report_type' => 'pdf',
        'ids' => array($this->rid),
      ),
      'split',
    );

    return $this->wiz_exe($send);
  }

  public function run() {
    $wid = $this->wiz_exe(array('stock.partial_picking'), 'create');

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
    return $this->send($wid, $form);
  }
}