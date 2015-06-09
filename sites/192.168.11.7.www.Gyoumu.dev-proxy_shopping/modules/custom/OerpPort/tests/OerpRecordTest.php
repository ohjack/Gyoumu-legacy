<?php

require_once './includes/bootstrap.inc';
$_SERVER['REQUEST_METHOD'] = 'get';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

//module_load_include('inc', 'oerp', 'oerp');

class OerpRecordTest extends PHPUnit_Framework_TestCase
{
  private $vals;
  private $arch;

  public function setUp()
  {
    $this->vals = array(
      'origin' => 'UPS print test00',
      'note' => 'CANCELED: TEST',
      'address_id' =>
      array(
        'city' => 'Rancho Dominguez',
        'name' => 'ACI Express, Inc. Los Angeles Br',
        'zip' => '90220',
        'street2' => '( #270674194150 )',
        'id' => 1627,
        'phone' => '650-253-0000',
        'street' => '2218 E.Gladwick Street',
        'state_id' =>
        array(
          'country_id' =>
          array(
            'name' => 'United States',
            'id' => 224,
          ),
          'name' => 'CA',
          'id' => 1003,
        ),
        'email' => '',
      ),
      'weight' => 1,
      'name' => 'PACK1717',
      'date_done' => '2011-08-09 18:09:34',
      'carrier_id' =>
      array(
        'name' => 'UPS',
        'id' => 8,
      ),
      'x_cust_val' => 45,
      'date' => '2011-08-05 13:26:35',
      'id' => 1713,
      'move_lines' =>
      array(
        0 =>
        array(
          'note' => '',
          'id' => 1754,
          'product_id' => '[MD-00319] BENZ SLK-R170 01~03 行李箱把手飾條',
          'product_qty' => 1,
        ),
      ),
    );

    $this->arch = \Gulei\File\Helper::getPathContent('module', 'oerp', '/tests/xml/OerpRecordTest.xml');
  }

  protected function createStub()
  {
    /*
    $stub = $this->getMock(
      '\Oerp\Query\Record',
      array('getRecord'),
      array($this->arch, '', array('oerp:ids' => '[1713]'))
    );

    $stub->expects($this->any())
      ->method('getRecord')
      ->will($this->returnValue($this->vals));

    return $stub;
    */
    return new \Oerp\Query\Record($this->arch, '', array('oerp:ids' => '[1713]'));
  }

  public function testDump()
  {
    $stub = $this->createStub();

    $this->assertXmlStringEqualsXmlString(
      \Gulei\File\Helper::getPathContent('module', 'oerp', '/tests/xml/OerpRecordMapped.xml'),
      $stub->dump()
    );
  }

  public function testGetRecord()
  {
    $stub = $this->createStub();
    $this->assertEquals($this->vals, $stub->getRecord());
  }

  public function testAdaptValues()
  {
    $stub = $this->createStub();

    $vals = array(
      'origin' => 'UPS print test01',
      'name' => 'PACK1718',
      'date' => '2011-08-05 13:26:35',
      'date_done' => '',
      'address_id' =>
      array(
        'name' => 'Yeo Yen Ping',
        'email' => 'ping@k.com',
        'phone' => '0',
        'zip' => '550113',
        'street' => 'BLK 113 SERANGOON NORTH AVE 1',
        'street2' => '#01-565',
        'city' => 'Singapore',
        'state_id' =>
        array(
          'name' => '',
          'country_id' =>
          array(
            'name' => 'Singapore',
          ),
        ),
      ),
      'carrier_id' =>
      array(
        'name' => 'R.Mail',
      ),
      'weight' => '1',
      'x_cust_val' => '32',
      'note' => '',
      'move_lines' => '',
    );

    $exp = array(
      'origin' => 'UPS print test01',
      'name' => 'PACK1718',
      'date' => '2011-08-05 13:26:35',
      'address_id' =>
      array(
        'name' => 'Yeo Yen Ping',
        'email' => 'ping@k.com',
        'phone' => '0',
        'zip' => '550113',
        'street' => 'BLK 113 SERANGOON NORTH AVE 1',
        'street2' => '#01-565',
        'city' => 'Singapore',
        'state_id' =>
        array(
          'name' => '',
          'country_id' =>
          array(
            'name' => 'Singapore',
          ),
        ),
      ),
      'carrier_id' =>
      array(
        'name' => 'R.Mail',
      ),
      'weight' => 1,
      'x_cust_val' => 32,
      'note' => '',
    );

    $this->assertEquals($exp, $stub->adaptValues($vals));
  }

  public function testWriteMany2one()
  {
    $arch = <<<XML
<form model="res.partner">
  <param name="oerp:ids">[10]</param>
  <field name="property_product_pricelist">
    <field name="name"/>
  </field>
</form>
XML;

    $Rec = new \Oerp\Query\Record($arch);
    $val1 = $Rec->getRecord();
    $val2 = $val1;
    $val2['property_product_pricelist']['name'] = $val1['property_product_pricelist']['name'] . '#';

    $this->assertEquals(true, $Rec->send($val2));
    $this->assertEquals($val2, $Rec->getRecord());

    $Rec->send($val1);
    $this->assertEquals($val1, $Rec->getRecord());
  }

  public function testCreateMany2one()
  {
    $arch = <<<XML
<form model="res.partner">
  <param name="oerp:ids">[10]</param>
  <field name="property_product_pricelist">
    <field name="name"/>
    <field name="type"/>
    <field name="currency_id"/>
  </field>
</form>
XML;

    $Rec = new \Oerp\Query\Record($arch);
    $val1 = $Rec->getRecord();

    $newVal = array(
      'property_product_pricelist' => array(
        'id' => 0,
        'name' => 'UNITEST CREATED',
        'type' => 'sale',
        'currency_id' => 2
      )
    );
    $this->assertEquals(true, $Rec->send($newVal));

    $val2 = $Rec->getRecord();
    $this->assertNotEquals($val1['property_product_pricelist']['id'], $val2['property_product_pricelist']['id']);

    $Rec->send(
      array(
        'property_product_pricelist' => $val1['property_product_pricelist']['id']
      )
    );
    $this->assertEquals($val1, $Rec->getRecord());
  }

  public function testWriteOne2many()
  {
    $arch = <<<XML
<form model="res.partner">
  <param name="oerp:ids">[10]</param>
  <field name="address">
    <field name="name"/>
  </field>
</form>
XML;

    $Rec = new \Oerp\Query\Record($arch);
    $val = $Rec->getRecord();
    $this->assertNotEmpty($val);

    $newVal = $val;
    $newVal['address'][0]['name'] = 'TEST 1';
    $newVal['address'][1]['name'] = 'TEST 2';

    $this->assertEquals(true, $Rec->send($newVal));
    $this->assertEquals($newVal, $Rec->getRecord());

    $Rec->send($val);
    $this->assertEquals($val, $Rec->getRecord());
  }

  public function testCreateOne2many()
  {
    $arch = <<<XML
<form model="res.partner">
  <param name="oerp:ids">[10]</param>
  <field name="address">
    <field name="name"/>
  </field>
</form>
XML;

    $Rec = new \Oerp\Query\Record($arch);
    $val = $Rec->getRecord();
    $this->assertNotEmpty($val);

    $count = count($val['address']);

    $newVal = array(
      'address' => array(
        array(
          'id' => 0,
          'name' => 'UNITTEST CREATE',
        )
      )
    );

    $this->assertEquals(true, $Rec->send($newVal));
    $valAdded = $Rec->getRecord();
    $this->assertEquals($count + 1, count($valAdded['address']));
  }
}

?>