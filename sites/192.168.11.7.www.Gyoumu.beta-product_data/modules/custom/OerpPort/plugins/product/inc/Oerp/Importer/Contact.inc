<?php
namespace Oerp\Importer;

class Contact extends \Gulei\Importer\Arch\_Abstract
{
  private $fldDef;
  private $ref;
  private $refRemote;

  public function getArch()
  {
    return file_get_contents(__DIR__ . '/Contact.xml');
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
    
    $mapping = <<<XML
<tree>
  <field name="name" from="partner"/>
  <field name="x_regnum" from="reg_num"/>
  <field name="x_payment_note" from="payment"/>
  <field name="comment" from="note"/>
  <field name="address">
    <field name="_seq_" eval="1">return 0;</field>
    <field name="_seq_" eval="1">return 0;</field>
    <field name="_seq_">
      <field name="name" from="contact"/>
      <field name="phone" from="phone"/>
      <field name="fax" from="fax"/>
      <field name="email" from="email"/>
      <field name="street" from="address"/>
    </field>
  </field>
</tree>
XML;

    $mapper = new \Gulei\Mapper\Basic($buf->getRecordsAltered());
    return $mapper->exe($mapping);
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
      'description' => '',
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

  public function submit($form, &$form_state)
  {
//    $this->prepareRef();
    $this->recs = $this->getRecs($form);
    dpr($this->recs);die;
/*
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
    }
*/
  }
}