<?php
namespace Oerp\Product\Provider;

class Meta implements \Gulei\Provider\_Interface
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
  <field name="product_manager">
    <field name="login" string="Mngr"/>
  </field>
  <field name="default_code" string="SKU" select="1"/>
  <field name="name" string="Product Name" select="1"/>
  <field name="func:real_state" string="Status"/>
  <field name="product_tmpl_id">
    <field name="state" string="ERP Status"/>
    <field name="purchase_ok" string="Buyable"/>
    <field name="sale_ok" string="Sellable"/>
  </field>

  <translate>
    <func>
      <src name="cmd:Oerp\\Product\\Command\\toRealState"/>
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
      'oerp:criteria' => array(array('state', '<>', 'obsolete')),
      'no-blank-tail' => true,
      'flag' => array(
        'no-cache' => TRUE,
      )
    );

    $prov = new \Oerp\Provider\TreeRecords($param);
    return $prov;
  }


  public function getData() {
    $proc = new \Gulei\Processor\Basic();
    $proc->setDataProvider($this->prov);

    $bool_flds = array(
      'product_tmpl_id.purchase_ok',
      'product_tmpl_id.sale_ok',
    );

    $proc->addModifiers(
      array(
           new \Gulei\Modifier\VisualizeBoolean(null, null, $bool_flds)
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