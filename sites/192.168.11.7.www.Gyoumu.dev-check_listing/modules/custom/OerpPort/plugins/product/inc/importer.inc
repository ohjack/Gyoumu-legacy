<?php
namespace Oerp\Importer;

require_once __DIR__ . '/../../../include/reference.inc';

class UpdateProduct extends \AbstractCsvArchImporter
{
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

    <func>
      <src name="cmd:Oerp\\Command\\existValue">
        <arg>res.partner</arg>
        <arg/>
        <arg>name</arg>
      </src>
      <fld>Supplier</fld>
      <msg>
        Not found in database.
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
  </validate>
</tree>
XML;
  }

  public function submit($form, &$form_state)
  {
    $Ref = new \Oerp\Reference\ValuePair();
    $Ref
        ->addPair('res.users', 'login', 'id')
        ->addPair('product.product', 'default_code', 'id')
        ->addPair('res.partner', 'name', 'id')
        ->addPair('res.currency', 'code', 'id')
    ;

    $ref = $Ref->val();
//    dpr($ref);
    $buf = $this->getBufferObj($form);
//    dpr($buf->getRecords());

    $fld_def = \OerpFields::getDef('product.product');

    foreach($buf->getRecords() as $rec){
      list($pid, $user, $sup_id) = array(
        $ref['product.product'][$rec['SKU']],
        $ref['res.users'][$rec['Owner']],
        $ref['res.partner'][$rec['Supplier']],
      );

      $send = array(
        'default_code' => $rec['SKU'],
        'name' => $rec['Name'],
//        'state' => $rec['Status'],
        'seller_ids' => array(
          array(
            0, 0,
            array(
              'name' => $sup_id,
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
        'product_manager' => $user,
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

      $flds = array_keys($send);

      /* TOFIX query batch */
      $q = new \OerpQuery('product.product', null, $flds, $pid);

      $rec_remote = $q->raw();
      $rec_remote = $rec_remote[0];

      $flush = array();

      foreach($flds as $fld_name){
        $fld_val = $rec_remote[$fld_name];
        
        if(!empty($fld_val) && $fld_def[$fld_name]['type'] === 'one2many'){
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
        $q->write($flush);
      }
      $q->write($send);
    }
  }
}