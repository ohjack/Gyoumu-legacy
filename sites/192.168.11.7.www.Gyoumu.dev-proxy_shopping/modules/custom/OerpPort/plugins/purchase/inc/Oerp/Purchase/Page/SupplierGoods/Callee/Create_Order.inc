<?php
namespace Oerp\Purchase\Page\SupplierGoods\Callee;

class Create_Order extends \Gulei\Page\Form\Callee\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function getEleOverrides()   {
    return array(
      '#attributes' => array('class' => 'needConfirm hideUnselected')
    );
  }

  public function execute($form, &$form_state) {
    $vals = $form_state['clicked_button']['#post']['view']['json'];
    $vals = json_decode($vals);

    $arch = <<<XML
<tree>
  <param name="oerp:model">product.supplierinfo</param>
  <field name="name">
    <field name="id"/>
    <field name="address">
      <field name="id"/>
    </field>
    <field name="property_product_pricelist">
      <field name="id"/>
    </field>
  </field>
  <field name="product_id">
    <field name="id"/>
    <field name="name"/>
    <field name="uom_id">
      <field name="id"/>
    </field>
  </field>
  <field name="pricelist_ids">
    <field name="min_quantity"/>
    <field name="price"/>
  </field>
</tree>
XML;

    $q = new \Oerp\Query\Nested($arch, array('oerp:ids' => array_keys((array)$vals)));
    $ref = $q->sum();

    $send = array(
      'partner_id' => $ref[0]['name']['id'],
      'partner_address_id' => $ref[0]['name']['address'][0]['id'],
      'pricelist_id' => $ref[0]['name']['property_product_pricelist']['id'],
    );

    $date = date('Y-m-d H:i:s');

    foreach ($ref as $info) {
      $qty = $vals->{$info['id']};

      if ($qty > 0) {
        $prod = $info['product_id'];
        $moq = -1;

        foreach ($info['pricelist_ids'] as $plist) {
          if ($plist['min_quantity'] > $moq) {
            $moq = $plist['min_quantity'];
            $uprz = $plist['price'];
          }
        }

        $line = array(
          'name' => $prod['name'],
          'product_uom' => $prod['uom_id']['id'],
          'date_planned' => $date,
          'price_unit' => (float)$uprz,
          'product_qty' => (float)$qty,
          'product_id' => $prod['id'],
        );

        $send['order_line'][] = array(0, 0, $line);
      }
    }

    $q = new \Oerp\Query('purchase.order');
    $new_id = $q->create($send);

    $msg = sprintf(
      'New purchase order created. Click <a target="_blank" href="?q=oerp/view/form/purchase.order/0/%s">here</a> to view it.',
      $new_id
    );
    drupal_set_message($msg);

    return TRUE;
  }
}