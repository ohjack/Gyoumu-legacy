<?php
namespace Oerp\Product\Page;

class InDev extends \Oerp\Page\TreeView
{
  protected function hook_form_internal()
  {
    $arch = <<<XML
<tree string="Product in Development">
  <param name="oerp:model">product.product</param>
  <param name="oerp:criteria">[["state","=","draft"]]</param>
  <param name="oerp:op-type">XUX</param>

  <field name="product_manager" string="Prod.M"/>
  <field name="default_code" string="SKU"/>
  <field name="name" string="Name"/>
  <field name="state" string="Status"/>
  <field name="func:ready_date" string="Ready Date"/>
  <field name="qty_available" string="Stock"/>
  <field name="packaging">
    <field name="ul" string="Packaging"/>
  </field>
  <field name="func:pic_fixed" string="Pic.Fixed"/>
  <field name="func:pic_published" string="FTP(titosy)"/>

  <translate>
    <func>
      <src name="cmd:Oerp\Product\Command\checkPicFixed"/>
      <fld>func:pic_fixed</fld>
    </func>

    <func>
      <src name="cmd:Oerp\Product\Command\checkPicPublished"/>
      <fld>func:pic_published</fld>
    </func>
  </translate>
</tree>
XML;

    $this->args = array(0, array('arch' => $arch));
    return parent::hook_form_internal();
  }
}