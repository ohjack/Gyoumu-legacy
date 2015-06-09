<?php
namespace Oerp\Delivery\Printer\FedEx;

class Call extends \Oerp\Delivery\Printer\Abstract_Call
{
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
    $h = new \Oerp\Delivery\Helper();
    return $h->getSkuDesc($rec);
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
      'comment' => "02 G5 NO BRAND\nContact 03-4801776 Miss Cheng if you have any questions.",
      'pay_type_trans' => '1',
      'pay_type_tax' => '2',
      'pkg_num' => '1',
    );
  }

  private function getSampleIn($with_dim = FALSE)
  {
    if ($with_dim) {
      $sample_src = file_get_contents(__DIR__ . '/sample_with_dim.in');
    }
    else {
      $sample_src = file_get_contents(__DIR__ . '/sample.in');
    }
    return $sample_src;
  }

  private function prepareSend($rec, $pkg)
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

    $send['pkg_weight'] = ($tmp = $pkg['weight']) ? $tmp / 1000: $rec['weight'];

    $send = array_merge($this->getSendTemplate(), $send);
    $send['com_desc1'] = $this->getDesc($rec);

    $send['pkg_weight'] = (int)((float)$send['pkg_weight'] * 10);
    $send['custom_value'] = (int)((float)$send['custom_value'] * 100);

    $ccode = $send['recp_country_code'];
    if($ccode != 'US' && $ccode != 'CA' && $send['recp_state']){
      $send['recp_state'] = '';
    }

    $send['service_type'] = $this->getServiceType($send['service_type']);

    $pack = $this->getPack($rec);
    $send['adm_pkg_type']= $pack->adm_pkg_type;

    if ($pkg['length']) {
      if (stripos($pkg['name'], 'fedex') === 0) {
        switch ($pkg['name']) {
          case 'FedEx Small Box':
          case 'FedEx Medium Box':
          case 'FedEx Large Box':
            $send['pkg_type'] = '03';
            break;

          case 'FedEx Tube':
            $send['pkg_type'] = '04';
            break;
        }
        $in = $this->getSampleIn();
      }
      else {
        $send['pkg_type'] = '01';
        $send['pkg_length'] = (int)$pkg['length'];
        $send['pkg_width'] = (int)$pkg['width'];
        $send['pkg_height'] = (int)$pkg['height'];
        $in = $this->getSampleIn(TRUE);
      }
    }
    else {
      $send['pkg_type'] = $pack->pkg_type;
      $in = $this->getSampleIn();
    }

    foreach($send as $name => $val){
      $in = str_replace('{$' . $name . '}', $val, $in);
    }
    return $in;
  }

  protected function _print($rec, $pkg)
  {
    $h = new \Oerp\Delivery\FedEx\Helper();
    $in = $this->prepareSend($rec, $pkg);

    $fname = $h->getFilename($rec['name'], false);
    $out = sprintf('%s/out/%s.out', $h->getListenerPath(), $fname);

    if(file_exists($out)){
      unlink($out);
    }

    $fp_in = sprintf('%s/in/%s.in', $h->getListenerPath(), $fname);
    return file_put_contents($fp_in, $in);
  }

  private function getServiceType($srv)
  {
    static $mapping = array(
      'Fedex IE' => 3,
      'Fedex IP' => 1,
    );
    return $mapping[$srv];
  }
}