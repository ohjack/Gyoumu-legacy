<?php
namespace Oerp\Delivery\Printer\FedEx;

class Call extends \Oerp\Delivery\Printer\FedEx\_Abstract
{
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

  private function getMappingArch()
  {
    return <<<XML
<mapping>
  <field name="id" from="id"/>
  <field name="tid" from="name"/>
  <field name="recp_email" from="address_id.email"/>
  <field name="recp_contact" from="address_id.name"/>
  <field name="recp_phone" from="address_id.phone"/>
  <field name="recp_addr1" from="address_id.street"/>
  <field name="recp_addr2" from="address_id.street2"/>
  <field name="recp_city" from="address_id.city"/>
  <field name="recp_post_code" from="address_id.zip"/>
  <field name="recp_state" from="address_id.state_id.name"/>
  <field name="recp_country_code" from="address_id.state_id.country_id.code"/>
  <field name="ref_note" from="origin"/>
  <field name="pkg_type" from="func:pkg_type"/>
  <field name="adm_pkg_type" from="func:adm_pkg_type"/>
  <field name="pkg_weight" from="weight"/>
  <field name="customs_value" from="x_cust_val"/>
  <field name="service_type" from="carrier_id"/>
  <field name="com_desc1" from="func:com_desc1"/>
</mapping>
XML;
  }

  private function getDesc($rec)
  {
    static $mapping;

    if(!$mapping){
      $csvDesc = new \Gulei\Csv\Model(array(
        'filepath' => __DIR__ . '/SkuDesc.csv',
      ));

      $mapping = array();
      foreach($csvDesc->connect() as $each){
        $pat = '/^' . trim($each['Category']) . '-.*/';
        $mapping[$pat] = $each['Description'];
      }
    }

    $skus = array();
    $qtys = array();

    foreach($rec['move_lines'] as $line){
      $skus[] = $line['product_id']['default_code'];
      $qtys[] = $line['product_qty'];
    }

    $pairs = array();
    foreach($skus as $ind => $sku){
      $desc = preg_replace(array_keys(($mapping)), $mapping, $sku);

      if($desc == $sku){
        $desc = 'Car Spoiler';
      }

      if(isset($pairs[$desc])){
        $pairs[$desc] += $qtys[$ind];
      }
      else{
        $pairs[$desc] = $qtys[$ind];
      }
    }

    foreach($pairs as $desc => &$pair){
      $pair = $desc . '(' . $pair . ')';
    }

    return implode(',', $pairs);
  }

  function getPack($rec)
  {
    static $mapping;

    if(!$mapping){
      $csvPkg = new \Gulei\Csv\Model(array(
        'filepath' => __DIR__ . '/SkuPackaging.csv',
      ));

      $ArryMapping = new \Gulei\_Array\Basic($csvPkg->connect());
      $mapping = (array)$ArryMapping->promote('SKU');
    }

    $skus = explode("\n", $rec["move_lines.product_id.default_code"]);
    $qtys = explode("\n", $rec["move_lines.product_qty"]);

    if(count($skus) > 1 || $qtys[0] > 1 || !isset($mapping[$skus[0]])){
      $pack = array('01', 'PCS');
    }
    else{
      $pkg_map = array(
        'Fedex Box' => array('03','BOX'),
        'Fedex Tube' => array('04', 'TBE'),
      );

      $type = $mapping[$skus[0]]['packaging'];
      $pack = $pkg_map[$type];

      if(!$pack){
        throw new \Exception('getFedexPackageType: Unknown packaging type.');
      }
    }
    return (object)array(
      'pkg_type' => $pack[0],
      'adm_pkg_type' => $pack[1],
    );
  }

  private function getSendTemplate()
  {
    return array(
      'recp_company' => '',
      'send_email' => 'export.lt@maolung.com',
      'weight_type' => 'KGS',
      'carriage_value' => '0',
      'sale_term' => '5',
      'currency' => 'USD',
      'com_mfr_country' => 'TW',
      'comment' => "02 G5 NO BRAND\nContact 03-4801776 Miss Hsu if you have any questions.",
      'pay_type_trans' => '1',
      'pay_type_tax' => '2',
      'pkg_num' => '1',
    );
  }

  private function getSampleIn()
  {
    static $sample_src = null;
    if(is_null($sample_src)){
      $sample_src = file_get_contents(__DIR__ . '/sample.in');
    }
    return $sample_src;
  }

  private function prepareSend($rec)
  {
    $addr = $rec['address_id'];

    $send = array(
      'tid' => $rec['name'],
      'recp_contact' => $addr['name'],
      'recp_country_code' => $addr['state_id']['country_id']['code'],
      'recp_addr1' => $addr['street'],
      'recp_addr2' => $addr['street2'],
      'recp_city' => $addr['city'],
      'recp_state' => $addr['state_id']['name'],
      'recp_post_code' => $addr['zip'],
      'recp_phone' => $addr['phone'],
      'recp_email' => $addr['email'],
      'ref_note' => $rec['origin'],
      'service_type' => $rec['carrier_id'],
      'pkg_weight' => $rec['weight'],
      'custom_value' => $rec['x_cust_val'],
    );

    $send = array_merge($this->getSendTemplate(), $send);
    $send['com_desc1'] = $this->getDesc($rec);

    $pack = $this->getPack($rec);
    $send['pkg_type'] = $pack->pkg_type;
    $send['adm_pkg_type']= $pack->adm_pkg_type;

    $send['pkg_weight'] = (int)((float)$send['pkg_weight'] * 10);
    $send['custom_value'] = (int)((float)$send['custom_value'] * 100);

    $ccode = $send['recp_country_code'];
    if($ccode != 'US' && $ccode != 'CA' && $send['recp_state']){
      $send['recp_state'] = '';
    }

    $send['service_type'] = $this->getServiceType($send['service_type']);

    $in = $this->getSampleIn();
    foreach($send as $name => $val){
      $in = str_replace('{$' . $name . '}', $val, $in);
    }
    return $in;
  }

  private function _print($rec)
  {
    $in = $this->prepareSend($rec);
    $fpath = sprintf('%s/in/%s', self::LISTENERPATH, $this->getFilename($rec['name'], 'in'));
    return file_put_contents($fpath, $in);
  }

  private function getServiceType($srv)
  {
    static $mapping = array(
      'Fedex IE' => 3,
      'Fedex IP' => 1,
    );
    return $mapping[$srv];
  }

  public function printOrders()
  {
    $ids = $this->prov->getData()->collect('id');
    $q = new \Oerp\Query\Nested($this->getQueryArch(), array('oerp:ids' => $ids));
    $recs = $q->sum();

    $qRec = new \Oerp\Query\Basic('stock.picking');

    foreach($recs as $rec) {
      if($this->_print($rec)){
        $qRec->setIds($rec['id']);
        $qRec->write(array(
          'x_tnum' => '_PRINT_',
        ));
      }
    }
  }
}