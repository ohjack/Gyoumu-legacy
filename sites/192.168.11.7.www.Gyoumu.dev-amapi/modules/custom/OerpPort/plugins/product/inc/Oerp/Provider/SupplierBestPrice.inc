<?php
namespace Oerp\Provider;

class SupplierBestPrice implements \Gulei\InterfaceProvider
{
  private $prov;

  public function __construct()
  {
    $this->prov = $this->getProvider();
  }

  public function getArch()
  {
    return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<tree name="view" string="Purchase Pricelist">
  <param name="oerp:model">product.product</param>

  <field name="default_code" string="SKU" select="1"/>
  <field name="name" string="Product Name" select="1"/>
  <field name="func:real_state" string="Status"/>
  <field name="product_tmpl_id">
    <field name="state" string="ERP Status"/>
    <field name="purchase_ok" string="Buyable"/>
    <field name="sale_ok" string="Sellable"/>
    <field name="seller_ids">
      <field name="product_code" string="P/N"/>
      <field name="name">
        <field name="name" string="Supplier"/>
        <field name="property_product_pricelist_purchase">
          <field name="currency_id" string="Curr."/>
        </field>
      </field>
      <field name="pricelist_ids">
        <field name="price" string="Price"/>
        <field name="min_quantity" string="MoQ"/>
      </field>
    </field>
  </field>

  <translate>
    <func>
      <src name="cmd:Oerp\\Command\\toRealState"/>
      <fld>func:real_state</fld>
    </func>
  </translate>
</tree>
XML;
  }

  private function getProvider()
  {
    $param = array(
      'arch' => $this->getArch(),
    );

    $prov = new TreeRecords($param);
    return $prov;
  }


  public function getData()
  {
    $proc = new \Gulei\Processor\Basic();
    $proc->setDataProvider($this->prov);

    $bool_flds = array(
      'product_tmpl_id.purchase_ok',
      'product_tmpl_id.sale_ok',
    );

    $proc->addModifiers(
      array(
           new \Oerp\Modifier\BestPrice(),
           new \Gulei\Modifier\VisualizeBoolean(null, null, $bool_flds)
      )
    );
    return $proc->getOutput();
  }

  public function getReference()
  {
    return $this->prov->getReference();
  }

  public function count()
  {
    return $this->prov->count();
  }
}