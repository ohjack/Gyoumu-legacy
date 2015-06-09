<?php
namespace Oerp\Importer;

class ProductData extends \Gulei\Importer\Arch\_Abstract
{
  private $fldDef;
  private $ref;
  private $refRemote;

  public function getArch()
  {
    return <<<XML
<tree string="Update Product Data">
  <field name="Owner"/>
  <field name="SKU"/>
  <field name="Status"/>
  <field name="Name"/>
  <field name="Supplier"/>
  <field name="Currency"/>

  <translate>
    <func>
      <src name="func:trim"/>
      <fld>Owner</fld>
      <fld>SKU</fld>
      <fld>Status</fld>
      <fld>Name</fld>
      <fld>Supplier</fld>
      <fld>Currency</fld>
    </func>
  </translate>

  <validate>
    <func>
      <src name="cmd:Oerp\\Command\\existValue">
        <arg>res.users</arg>
        <arg/>
        <arg>login</arg>
      </src>
      <fld>Owner</fld>
      <msg>
        Not found in database.
      </msg>
    </func>

    <func type="notice">
      <src name="cmd:Oerp\\Command\\existValue">
        <arg>res.partner</arg>
        <arg/>
        <arg>name</arg>
      </src>
      <fld>Supplier</fld>
      <msg>
        In-exist supplier will be created on-fly.
      </msg>
    </func>

    <func>
      <src name="cmd:Oerp\\Command\\existValue">
        <arg>res.currency</arg>
        <arg/>
        <arg>code</arg>
      </src>
      <fld>Currency</fld>
      <msg>
        Not found in database.
      </msg>
    </func>

    <func>
      <src name="cmd:Gulei\\Command\\isDistinct"/>
      <fld>SKU</fld>
      <msg>
        Duplicate value.
      </msg>
    </func>

    <func type="notice">
      <src name="cmd:Oerp\\Command\\existValue">
        <arg>product.product</arg>
        <arg/>
        <arg>default_code</arg>
      </src>
      <fld>SKU</fld>
      <msg>
        In-exist product will be created on-fly.
      </msg>
    </func>
  </validate>
</tree>
XML;
  }

  private function prepareRef()
  {
    $Ref = new \Oerp\Reference\ValuePair();
    $Ref
        ->addPair('res.users', 'login', 'id')
        ->addPair('product.product', 'default_code', 'id')
        ->addPair('res.partner', 'name', 'id')
        ->addPair('res.currency', 'code', 'id')
    ;
    $this->ref = $Ref->val();
  }

  private function getRecs($form)
  {
    $buf = $this->getBufferObj($form);
    return $buf->getRecordsAltered();
  }

  private function getRecIdPack($rec)
  {
    if(!$this->ref){
      $this->prepareRef();
    }

    return array(
      'pid' => $this->ref['product.product'][$rec['SKU']],
      'user' => $this->ref['res.users'][$rec['Owner']],
      'sup_id' => $this->ref['res.partner'][$rec['Supplier']],
    );
  }

  private function createMissingRef($rec)
  {
    $ids = $this->getRecIdPack($rec);

    if(!$ids['pid']){
      $q = new \Oerp\Query\Basic('product.product');
      $pid = $q->create(
        array(
          'name' => $rec['Name'],
          'default_code' => $rec['SKU'],
        )
      );
      $this->ref['product.product'][$rec['SKU']] = $pid;
      unset($q);
    }

    if(!$ids['sup_id']){
      $q = new \Oerp\Query\Basic('res.partner');
      $sup_id = $q->create(
        array(
          'name' => $rec['Supplier'],
          'customer' => false,
          'supplier' => true,
        )
      );
      $this->ref['res.partner'][$rec['Supplier']] = $sup_id;
      unset($q);
    }
  }

  private function getSendData($rec)
  {
    $ids = $this->getRecIdPack($rec);

    $send = array(
      'default_code' => $rec['SKU'],
      'name' => $rec['Name'],
      'seller_ids' => array(
        array(
          0, 0,
          array(
            'name' => $ids['sup_id'],
            'product_code' => $rec['P/N'],
            'pricelist_ids' => array(
              array(
                0, 0,
                array(
                  'min_quantity' => $rec['Main Moq'],
                  'price' => $rec['Main Cost'],
                )
              )
            )
          )
        ),
      ),
      'product_manager' => $ids['user'],
      'description' => $rec['Note Link'],
    );

    switch($rec['Status']){
      case 'Ready':
        $send['state'] = 'sellable';
        $send['purchase_ok'] = true;
        $send['sale_ok'] = true;
        break;

      case 'EoL':
        $send['state'] = 'end';
        $send['purchase_ok'] = true;
        $send['sale_ok'] = true;
        break;

      case 'Close':
        $send['state'] = 'sellable';
        $send['purchase_ok'] = false;
        $send['sale_ok'];
        break;

      case '開發中':
        $send['state'] = 'draft';
        $send['purchase_ok'] = true;
        $send['sale_ok'] = true;
        break;
    }
    return $send;
  }

  private function updateRefRemote($pids)
  {
    $flds = array_keys($this->getSendData($this->recs[0]));
    $q = new \Oerp\Query\Basic('product.product', null, $flds, $pids);
    $this->refRemote = $q->raw()->promote('id');
  }

  private function flushRec($rec, $send)
  {
    $id = $this->getRecIdPack($rec);
    $rec_remote = $this->refRemote[$id['pid']];

    if(!$this->fldDef){
      $this->fldDef = \Oerp\Query\Fields::getDef('product.product');
    }

    $flush = array();
    foreach(array_keys($send) as $fld_name){
      $fld_val = $rec_remote[$fld_name];

      if(!empty($fld_val) && $this->fldDef[$fld_name]['type'] === 'one2many'){
        $flush[$fld_name] = array();

        foreach($fld_val as $id_to_del){
          $each = array(
            2, $id_to_del, false
          );
          $flush[$fld_name][] = $each;
        }
      }
    }

    if($flush){
      $q = new \Oerp\Query\Basic('product.product', null, null, $id['pid']);
      $q->write($flush);
    }
  }

  private function setSupplierPricelist($type, $rec, $send)
  {
    static $plists;

    if(!$plists){
      $q = new \Oerp\Query\Basic('product.pricelist');
      $resp = new \Gulei\_Array\Basic($q->raw());
      $plists = $resp->bunch('type');
      unset($q, $resp);
    }

    $cur = $rec['Currency'];

    foreach($plists[$type] as $plist){
      if($plist['currency_id'][1] == $cur){
        $plist_id = $plist['id'];
        break;
      }
    }

    $fld_map = array(
      'purchase' => 'property_product_pricelist_purchase',
      'sale' => 'property_product_pricelist',
    );

    if(!$plist_id){
      $q = new \Oerp\Query\Basic('product.pricelist');
      $plist_id = $q->create(array(
        'currency_id' => $this->ref['res.currency'][$cur],
        'name' => ucfirst($type) . ' Pricelist',
        'type' => $type,
        'active' => true
      ));
    }

    $q = new \Oerp\Query\Basic('res.partner', null, null, $send['seller_ids'][0][2]['name']);
    $q->write(array($fld_map[$type] => $plist_id));
  }

  public function submit($form, &$form_state)
  {
    $this->prepareRef();
    $this->recs = $this->getRecs($form);

    $pids = array();
    foreach($this->recs as $rec) {
      $this->createMissingRef($rec);
      $id = $this->getRecIdPack($rec);
      $pids[] = $id['pid'];
    }

    $this->updateRefRemote($pids);

    foreach($this->recs as $rec){
      $id = $this->getRecIdPack($rec);
      $send = $this->getSendData($rec);
      $this->flushRec($rec, $send);

      $q = new \Oerp\Query\Basic('product.product', null, null, $id['pid']);
      $q->write($send);

      $this->setSupplierPricelist('purchase', $rec, $send);
      $this->setSupplierPricelist('sale', $rec, $send);
   }

    drupal_set_message(sprintf('%d record(s) updated.', count($this->recs)));
  }
}