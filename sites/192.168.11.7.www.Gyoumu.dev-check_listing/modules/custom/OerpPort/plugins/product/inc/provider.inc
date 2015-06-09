<?php
namespace Oerp;
use Gulei as G;

require_once __DIR__ . '/../../../include/provider.inc';

class ProviderSupplierBestPrice implements G\InterfaceProvider
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

    $prov = new ProviderTreeRecords($param);
    return $prov;
  }


  public function getData()
  {
    $proc = new \GuProcessor();
    $proc->setDataProvider($this->prov);

    $bool_flds = array(
      'product_tmpl_id.purchase_ok',
      'product_tmpl_id.sale_ok',
    );

    $proc->addModifiers(
      array(
           new ModBestPrice(),
           new ModVisualizeBoolean(null, null, $bool_flds)
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

class ModBestPrice extends G\AbstractModifier
{
  static private $prev_supp;

  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  private function getBestPricelist($pricelist, $rec)
  {
    $sup_key = 'product_tmpl_id.seller_ids.name.name';
    $keys = array(
      'qty' => 'product_tmpl_id.seller_ids.pricelist_ids.min_quantity',
      'price' => 'product_tmpl_id.seller_ids.pricelist_ids.price'
    );

    if(!is_null($rec[$sup_key])){
      self::$prev_supp = $rec[$sup_key];
    }

    if(is_null($pricelist[$keys['price']])
       || $rec[$keys['price']] < $pricelist[$keys['price']]){

      foreach($keys as $key){
        $pricelist[$key] = $rec[$key];
      }
      $pricelist[$sup_key] = self::$prev_supp;
    }
    return $pricelist;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $recs = &$RefVars->refVar('rows');

    $anchor = null;
    $pricelist = array();

    for($c = 0; $c < count($recs); $c++){
      if($recs[$c]['id']){
        $anchor = &$recs[$c];
        $pricelist = $this->getBestPricelist(array(), $recs[$c]);
      }
      else{
        $pricelist = $this->getBestPricelist($pricelist, $recs[$c]);
        $anchor = array_merge($anchor, $pricelist);
        unset($recs[$c]);
      }
    }
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}

class ModVisualizeBoolean extends G\AbstractModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    $cell = $RefVars->val('cell');
    return is_bool($cell);
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');
    $cell = $cell ? 'Yes' : 'No';
  }
}