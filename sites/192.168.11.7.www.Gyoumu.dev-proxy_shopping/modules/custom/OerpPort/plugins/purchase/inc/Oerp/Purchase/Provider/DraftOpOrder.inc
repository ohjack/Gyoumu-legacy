<?php
namespace Oerp\Purchase\Provider;

class DraftOpOrder extends \Gulei\Provider\_Abstract
{
  private function getQuery()
  {
    return new \Oerp\Query\Basic(
      'purchase.order',
      array(
        array('origin', 'like', 'OP/'),
        array('state', '=', 'draft')
      )
    );
  }

  public function getData()
  {
    $arch = <<<XML
<tree>
  <param name="oerp:model">purchase.order</param>
  <field name="id"/>
  <field name="name"/>
  <field name="state"/>
  <field name="order_line">
    <field name="name"/>
    <field name="product_id">
      <field name="state"/>
      <field name="purchase_ok"/>
      <field name="default_code"/>
    </field>
    <field name="product_qty"/>
  </field>
</tree>
XML;

    $param = array(
      'oerp:criteria' => array(
        array('state', '=', 'draft'),
      ),
    );

    $flags = array(
      'no-selection-name' => TRUE,
    );

    $q = new \Oerp\Query\Nested($arch, $param);
    return $q->sum($flags);
  }

  public function count()
  {
    return $this->getQuery()->count();
  }

  public function getReference()
  {
    return array();
  }
}