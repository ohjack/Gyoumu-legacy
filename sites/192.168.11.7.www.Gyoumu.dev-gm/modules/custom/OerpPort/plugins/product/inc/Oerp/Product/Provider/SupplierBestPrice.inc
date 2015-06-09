<?php
namespace Oerp\Product\Provider;

class SupplierBestPrice implements \Gulei\Provider\_Interface
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
  <field name="product_tmpl_id">
    <field name="seller_ids">
      <field name="product_code" string="P/N"/>
      <field name="name">
        <field name="name" string="Supplier"/>
        <field name="property_product_pricelist_purchase">
          <field name="currency_id" string="Curr."/>
        </field>
      </field>n
      <field name="pricelist_ids">
        <field name="price" string="Price"/>
        <field name="min_quantity" string="MoQ"/>
      </field>
    </field>
  </field>
</tree>
XML;
  }

  private function getProvider()
  {
    $param = array(
      'arch' => $this->getArch(),
      'oerp:criteria' => array(array('state', '<>', 'obsolete'))
    );

    $prov = new \Oerp\Provider\TreeRecords($param);
    return $prov;
  }


  public function getData()
  {
    $proc = new \Gulei\Processor\Basic();
    $proc->setDataProvider($this->prov);

    $proc->addModifiers(
      array(
           new \Oerp\Product\Modifier\BestPrice(),
      )
    );

    $recs = $proc->getOutput();
    \Oerp\Product\_Array\Command\sortSKU::sort($recs);
    return $recs;
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