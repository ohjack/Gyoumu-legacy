<?php
namespace Oerp\Product\Provider;

class Meta extends \Oerp\Product\Provider\Abstract_Meta
{
  public function getArch()
  {
    return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<tree name="view" string="Purchase Pricelist">
  <param name="oerp:model">product.product</param>
  <field name="product_manager">
    <field name="login" string="Mngr"/>
  </field>
  <field name="default_code" string="SKU" select="1"/>
  <field name="name" string="Product Name" select="1"/>
  <field name="product_tmpl_id">
    <field name="func:real_state" string="Status"/>
    <field name="state" string="ERP Status"/>
    <field name="purchase_ok" string="Buyable"/>
    <field name="sale_ok" string="Sellable"/>
  </field>

  <translate>
    <func>
      <src name="cmd:Oerp\\Product\\Command\\toRealState"/>
      <fld>product_tmpl_id.func:real_state</fld>
    </func>
  </translate>
</tree>
XML;
  }
}