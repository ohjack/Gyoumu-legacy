<?php

require_once './includes/bootstrap.inc';
$_SERVER['REQUEST_METHOD'] = 'get';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

class GuArrayTest extends PHPUnit_Framework_TestCase
{
  private $recs;
  private $array;

  public function setUp()
  {
    $this->recs = array(
      array(
        'seller' => 'euromotorsport',
        'sku' => 'MD-00319',
        'qty' => '1',
        'buyer' => 'ACI Express, Inc. Los Angeles Br',
        'phone' => '0',
        'addr1' => '2218 E.Gladwick Street',
        'addr2' => '( #270674194150 )',
        'city' => 'Rancho Dominguez',
        'state' => 'CA',
        'zip' => '90220',
        'country' => 'US',
      ),
      array(
        'seller' => 'euromotorsport',
        'sku' => 'LL-00329',
        'qty' => '1',
        'buyer' => 'Yeo Yen Ping',
        'phone' => '0',
        'addr1' => 'BLK 113 SERANGOON NORTH AVE 1',
        'addr2' => '#01-565',
        'city' => 'Singapore',
        'state' => '',
        'zip' => '550113',
        'country' => 'SG',
      )
    );

    $this->array = new GuArray($this->recs);
  }

  public function testInit()
  {
    $this->assertNotEmpty($this->recs);
    $this->assertEquals($this->recs, (array)$this->array);
  }

  public function testMappingFlat()
  {
    $mapping = <<<XML
<mapping>
  <field name="shop" from="seller"/>
</mapping>
XML;

    $expected = array(
      array('shop' => 'euromotorsport'),
      array('shop' => 'euromotorsport')
    );

    $mapped = $this->array->mapping($mapping);
    $this->assertEquals($expected, $mapped);
  }

  public function testMappingNestedLv1()
  {
    $mapping = <<<XML
<mapping>
  <field name="buyer">
    <field name="name" from="buyer"/>
    <field name="phone" from="phone"/>
  </field>
</mapping>
XML;

    $expected = array(
      array(
        'buyer' =>
        array(
          'name' => 'ACI Express, Inc. Los Angeles Br',
          'phone' => '0',
        )
      ),
      array(
        'buyer' =>
        array(
          'name' => 'Yeo Yen Ping',
          'phone' => '0',
        )
      )
    );

    $mapped = $this->array->mapping($mapping);
    $this->assertEquals($expected, $mapped);
  }

  public function testMappingNestedLv2()
  {
    $mapping = <<<XML
<mapping>
  <field name="buyer">
    <field name="name" from="buyer"/>
    <field name="phone" from="phone"/>
    <field name="addr">
      <field name="addr1" from="addr1"/>
      <field name="addr2" from="addr2"/>
    </field>
  </field>
</mapping>
XML;

    $expected = array(
      array(
        'buyer' =>
        array(
          'name' => 'ACI Express, Inc. Los Angeles Br',
          'phone' => '0',
          'addr' => array(
            'addr1' => '2218 E.Gladwick Street',
            'addr2' => '( #270674194150 )',
          )
        )
      ),
      array(
        'buyer' =>
        array(
          'name' => 'Yeo Yen Ping',
          'phone' => '0',
          'addr' => array(
            'addr1' => 'BLK 113 SERANGOON NORTH AVE 1',
            'addr2' => '#01-565',
          )
        )
      )
    );

    $mapped = $this->array->mapping($mapping);
    $this->assertEquals($expected, $mapped);
  }

  public function testMappingNestedComplex()
  {
    $mapping = <<<XML
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
  </field>
</mapping>

XML;

    $expected = array(
      array(
        'x_seller' => 'euromotorsport',
        'origin' => NULL,
        'x_cust_val' => NULL,
        'carrier_id' => NULL,
        'address_id' =>
        array(
          'email' => NULL,
          'name' => 'ACI Express, Inc. Los Angeles Br',
          'phone' => '0',
          'street' => '2218 E.Gladwick Street',
          'street2' => '( #270674194150 )',
          'city' => 'Rancho Dominguez',
          'zip' => '90220',
          'state_id' =>
          array(
            'name' => 'CA',
            'country_id' => 'US',
          ),
        ),
        'move_lines' =>
        array(
          'product_qty' => '1',
          'product_id' => 'MD-00319',
          'note' => NULL,
        ),
      ),
      array(
        'x_seller' => 'euromotorsport',
        'origin' => NULL,
        'x_cust_val' => NULL,
        'carrier_id' => NULL,
        'address_id' =>
        array(
          'email' => NULL,
          'name' => 'Yeo Yen Ping',
          'phone' => '0',
          'street' => 'BLK 113 SERANGOON NORTH AVE 1',
          'street2' => '#01-565',
          'city' => 'Singapore',
          'zip' => '550113',
          'state_id' =>
          array(
            'name' => '',
            'country_id' => 'SG',
          ),
        ),
        'move_lines' =>
        array(
          'product_qty' => '1',
          'product_id' => 'LL-00329',
          'note' => NULL,
        ),
      ),
    );

    $mapped = $this->array->mapping($mapping);
    $this->assertEquals($expected, $mapped);
  }
}

?>