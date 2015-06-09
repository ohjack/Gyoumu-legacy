<?php
namespace Oerp\Delivery\Importer;

class NewOrder extends \Gulei\Importer\Arch\_Abstract {
  protected $StateCtrl;

  protected function getClassname() {
    return __CLASS__;
  }

  protected function getLocalArch($fname) {
    return file_get_contents(__DIR__ . '/NewOrder/' . $fname);
  }

  public function getArch() {
    return $this->getLocalArch('input.xml');
  }

  protected function getRecDate() {
    return date('Y-m-d H:i:s');
  }

  protected function prepareSend(&$recs) {
    $mapping = <<<EOS
    <mapping>
      <field name="x_seller" from="seller"/>
      <field name="origin" from="tid"/>
      <field name="x_cust_val" from="value"/>
      <field name="carrier_id" from="carrier"/>
      <field name="address_id">
        <field name="email" from="email"/>
        <field name="name" from="buyer"/>
        <field name="phone" from="phone"/>
        <field name="street" from="addr1"/>
        <field name="street2" from="addr2"/>
        <field name="city" from="city"/>
        <field name="zip" from="zip"/>
        <field name="state_id">
          <field name="name" from="state"/>
          <field name="country_id" from="country"/>
        </field>
      </field>
      <field name="move_lines">
        <field name="product_qty" from="qty"/>
        <field name="product_id" from="sku"/>
        <field name="note" from="remark"/>
        <field name="x_saleprice" from="saleprice"/>
        <field name="x_currency" from="currency"/>
      </field>
    </mapping>
EOS;

    $queryProduct = new \Oerp\Query\Basic('product.product', null, array('default_code'));
    $refProducts = $queryProduct->raw()->promote('default_code');

    $queryCountry = new \Oerp\Query\Basic('res.country', null, array('code'));
    $refCountry = $queryCountry->raw()->promote('code');

    $queryCarrier = new \Oerp\Query\Basic('delivery.carrier', null, array('name'));
    $refCarrier = $queryCarrier->raw()->promote('name');

    $queryState = new \Oerp\Query\Basic('res.country.state', null, array('name', 'country_id'));

    $refState = array();
    foreach($queryState->raw() as $state){
      $refState[$state['country_id'][0] . '-' . $state['name']] = $state['id'];
    }

    $recs = $recs->mapping($mapping);

    foreach($recs as &$rec){
      $sku = $rec['move_lines']['product_id'];
      $country = $rec['address_id']['state_id']['country_id'];
      $carrier = $rec['carrier_id'];

      if(!isset($refProducts[$sku])){
        $q = new \Oerp\Query\Basic('product.product', array(array('default_code', '=', $sku)), array('id'));
        $resp = $q->create(
          array(
            'default_code' => $sku,
            'name' => '_UNCONFIRMED_',
          )
        );
        $refProducts[$sku]['id'] = $resp;
      }

      $rec['type'] = 'delivery';
      $rec['date'] = $this->getRecDate();
      $rec['address_id']['partner_id'] = array(
        'name' => $rec['address_id']['name'],
        'customer' => TRUE,
        'supplier' => FALSE,
      );

      $state_id = $refState[$refCountry[$country]['id'] . '-' . $rec['address_id']['state_id']['name']];

      if($state_id){
        $rec['address_id']['state_id'] = (int)$state_id;
      }
      else{
        $rec['address_id']['state_id']['code'] = 0;
        $rec['address_id']['state_id']['country_id'] = $refCountry[$country]['id'];
      }

      $rec['move_lines']['name'] = $sku;
      $rec['move_lines']['product_uom'] = 1;
      $rec['move_lines']['location_id'] = 11;
      $rec['move_lines']['location_dest_id'] = 8;
      $rec['move_lines']['product_id'] = $refProducts[$sku]['id'];
      $rec['carrier_id'] = $refCarrier[$carrier]['id'];
      $rec['weight'] = ($rec['weight']) ? $rec['weight'] : 0.5;

      $rec['move_lines'] = array($rec['move_lines']);
    }
  }

  public function submit($form, &$form_state) {
    $this->StateCtrl = new \Gulei\Form\State($form_state);
    $recs = new \Gulei\_Array\Basic(
      $this->getRecs($form, array('no_validate' => true)));

    $this->prepareSend($recs);

    $send_src = $this->getLocalArch('send.xml');
    $Rec = new \Oerp\Query\Record($send_src);

    foreach($recs as $rec){
      $Rec->send($rec);
    }

    $errs = form_get_errors();
    $warns = drupal_get_messages('warning', false);

    if(empty($errs) && empty($warns)){
      drupal_set_message(count($recs) . ' records submitted.');
    }
    drupal_goto('oerp/delivery/list/draft');
  }
}