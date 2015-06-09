<?php
namespace Oerp\Product\Provider;

class Records implements \Gulei\Provider\_Interface
{
  public function getViewArch()
  {
    $arch = <<<XML
<tree string="Product in Development">
  <param name="oerp:model">product.product</param>
  <param name="oerp:op-type">XUX</param>

  <field name="product_manager" string="Prod.M"/>
  <field name="default_code" string="SKU"/>
  <field name="name" string="Name"/>
  <field name="state" string="Status"/>
  <field name="func:ready_date" string="Ready Date"/>
  <field name="qty_available" string="Stock"/>
  <field name="func:pic_fixed"/>
  <field name="func:pic_published"/>
</tree>
XML;
    return $arch;
  }

  public function getReference()
  {
    return array();
  }

  private function getArchOrder($id)
  {
    return <<<XML
<tree>
  <param name="oerp:model">purchase.order.line</param>
  <param name="oerp:criteria">[["product_id", "=", {$id}]]</param>

  <field name="order_id">
    <field name="name" string="Ref"/>
    <field name="state" string="Status"/>
    <field name="date_order" string="Created"/>
    <field name="minimum_planned_date" string="Planned"/>
    <field name="partner_id" string="Supplier"/>
  </field>
</tree>
XML;
  }

  private function getOrder($id)
  {
    $q = new \Oerp\Query\Nested($this->getArchOrder($id));
    $send = array();

    foreach($q->sum() as $order){
      $send[] = $order['order_id'];
    }
    return $send;
  }

  private function getOrderFields()
  {
    $Ele = new \Oerp\Arch\Element($this->getArchOrder(0));
    return array_keys($Ele->extractAttr('name'));
  }

  private function getArchProduct()
  {
    return <<<XML
<tree>
  <param name="oerp:model">product.product</param>
  <param name="oerp:criteria">[["state", "=", "draft"]]</param>

  <field name="id"/>
  <field name="product_manager"/>
  <field name="default_code"/>
  <field name="name"/>
  <field name="qty_available"/>
  <field name="state"/>
</tree>
XML;
  }

  private function getProductFields()
  {
    $Ele = new \Oerp\Arch\Element($this->getArchProduct());
    return array_keys($Ele->extractAttr('name'));
  }

  private function getProducts()
  {
    $param = array(
      'pager' => true,
      'items_per_page' => 80,
      'page' => 1,
    );

    $q = new \Oerp\Query\Nested($this->getArchProduct(), $param);
    return $q->sum();
  }

  private function getAllFields()
  {
    return array_merge($this->getProductFields(), $this->getOrderFields());
  }

  public function getData()
  {
    $recs = $this->getProducts();
    return $recs;
  }

  public function count()
  {
    $q = new \Oerp\Query\Basic('product.product', array('state' => 'draft'), array('id'));
    return count($q->raw());
  }
}