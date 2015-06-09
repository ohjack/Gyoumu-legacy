<?php
namespace Oerp\Delivery\Provider\ReportPrint;

class Picking_List extends \Oerp\Delivery\Provider\ReportPrint\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function getTable() {
    $Tb = parent::getTable();
    $recs = $Tb->getRecords();
    $recs_send = array();

    foreach ($recs as $rec) {
      $sku = $rec['move_lines.product_id.default_code'];
      $qty = $rec['move_lines.product_qty'];

      if (!isset($recs_send[$sku])) {
        $recs_send[$sku] = array(
          'sku' => $sku,
          'qty' => 0,
        );
      }
      $recs_send[$sku]['qty'] += $qty;
    }

    $arch = <<<XML
<tree>
  <param name="class">print</param>

  <field name="sku" string="SKU"/>
  <field name="qty" String="Qty."/>
</tree>
XML;

    return new \Gulei\Table\Arch($arch, new \Gulei\Provider\Lambda($recs_send));
  }

  public function getThemeSrc() {
    $buf = '';
    $buf .= str_replace('|', ', ', $_GET['carrier']);
    $buf .= $this->getTable()->theme();
    return sprintf('<div style="width: 400px;">%s</div>', $buf);
  }
}