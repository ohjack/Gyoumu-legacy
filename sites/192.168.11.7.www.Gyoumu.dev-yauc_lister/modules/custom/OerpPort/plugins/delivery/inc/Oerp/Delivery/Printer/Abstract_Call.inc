<?php
namespace Oerp\Delivery\Printer;

abstract class Abstract_Call
{
  abstract protected function _print($rec, $pkg);

  private function getQueryArch()
  {
    return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<tree model="stock.picking" string="Deliver Order">

  <param name="copy_method">merge</param>

  <field name="name"/>
  <field name="origin" to="tid" string="Order ID" colspan="4"/>
  <field name="carrier_id"/>
  <field name="address_id" colspan="4" col="6">
    <field name="email"/>
    <field name="name"/>
    <field name="phone"/>
    <field name="street"/>
    <field name="street2"/>
    <field name="city"/>
    <field name="zip"/>
    <field name="state_id">
      <field name="name"/>
      <field name="country_id">
        <field name="code"/>
      </field>
    </field>
  </field>
  <field name="x_cust_val"/>
  <field name="move_lines" colspan="4">
    <field name="product_id">
      <field name="default_code"/>
    </field>
    <field name="product_qty"/>
  </field>
  <field name="weight"/>
</tree>
XML;
  }

  public function printOrders($prov)
  {
    $data = new \Oerp\Util\_Array($prov->getData());
    $ids = $data->collect('id');

    $q = new \Oerp\Query\Nested($this->getQueryArch(), array('oerp:ids' => $ids));
    $recs = $q->sum();

    $skus = array();
    foreach ($recs as $rec) {
      $skus[] = $rec['move_lines']['product_id']['default_code'];
    }

    $GetPkgType = new \Oerp\Delivery\Helper\GetPkgType($skus, $ids);

    $qRec = new \Oerp\Query\Basic('stock.picking');

    foreach($recs as $rec) {
      if($this->_print($rec, $GetPkgType->get($rec['id']))){
        $qRec->setIds($rec['id']);
        $qRec->write(array(
          'x_tnum' => '_PRINT_',
        ));
      }
    }
  }
}