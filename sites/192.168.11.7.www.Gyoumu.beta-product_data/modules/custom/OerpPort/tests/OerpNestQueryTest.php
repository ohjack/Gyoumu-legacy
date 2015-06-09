<?php

require_once './includes/bootstrap.inc';
$_SERVER['REQUEST_METHOD'] = 'get';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

class OerpNestQueryTest extends PHPUnit_Framework_TestCase
{
  protected $arch;
  protected $sum;

  public function setUp()
  {
    $this->arch = \Gulei\File\Helper::getPathContent('module', 'oerp', '/tests/xml/OerpNestQueryTest.xml');
  }

  protected function create()
  {
    return new \Oerp\Query\Nested($this->arch);
  }

  public function testCreate()
  {
    $this->assertNotEmpty($this->create());
  }

  public function testGetFields()
  {
    $exp = array(
      'origin', 'move_lines', 'carrier_id'
    );

    $q = $this->create();
    $this->assertEquals($exp, $q->getFields());
  }

  public function testRaw()
  {
    $exp = array(
      0 =>
      array(
        'origin' => 'UPS print test00',
        'id' => 1713,
        'carrier_id' =>
        array(
          0 => 8,
          1 => 'UPS',
        ),
        'move_lines' =>
        array(
          0 => 1754,
        ),
      ),
    );

    $q = $this->create();
    $this->assertEquals($exp, (array)$q->raw());
  }

  public function testInnerRaw()
  {
    $q = $this->create();

    $inner_arch = $q->dump(
      $q->xp('/form/field[@name="carrier_id"]')->item(0)
    );

    $raw = $q->raw();
    $ids = '[' . $raw[0]['carrier_id'][0] . ']';

    $inner_q = new \Oerp\Query\Nested(
      $inner_arch,
      array('oerp:ids' => $ids)
    );

    $this->assertEquals(array('name'), $inner_q->getFields());

    $exp = array(
      0 =>
      array(
        'name' => 'UPS',
        'id' => 8,
      ),
    );

    $this->assertEquals($exp, (array)$inner_q->raw());
  }

  public function testSum()
  {
    $exp = array(
      0 =>
      array(
        'origin' => 'UPS print test00',
        'id' => 1713,
        'carrier_id' =>
        array(
          'name' => 'UPS',
          'id' => 8,
        ),
        'move_lines' =>
        array(
          0 =>
          array(
            'product_id' => '[MD-00319] BENZ SLK-R170 01~03 行李箱把手飾條',
            'id' => 1754,
          ),
        ),
      ),
    );

    $q = $this->create();
    $this->assertEquals($exp, (array)$q->sum());
  }

  public function testSum2()
  {
    $arch = \Gulei\File\Helper::getPathContent('module', 'oerp', '/tests/xml/OerpNestQueryTest2.xml');
    $this->assertNotEmpty($arch);

    $q = new \Oerp\Query\Nested($arch);
    $this->assertNotEmpty($q);

    $exp = array(
      'product_id',
      'product_qty',
      'note'
    );

    $this->assertEquals($exp, $q->getFields());

    $exp = array(
      0 =>
      array(
        'note' => '',
        'id' => 1754,
        'product_id' => '[MD-00319] BENZ SLK-R170 01~03 行李箱把手飾條',
        'product_qty' => 1,
      ),
    );

    $this->assertEquals($exp, (array)$q->sum());
  }

  public function testSum3()
  {
    $arch = \Gulei\File\Helper::getPathContent('module', 'oerp', '/tests/xml/OerpNestQueryTest3.xml');
    $this->assertNotEmpty($arch);

    $q = new \Oerp\Query\Nested($arch);
    $this->assertNotEmpty($q);

    $exp = array(
      0 => 'x_seller',
      1 => 'name',
      2 => 'state',
      3 => 'date',
      4 => 'origin',
      5 => 'move_lines',
      6 => 'address_id',
      7 => 'x_cust_val',
      8 => 'carrier_id',
      9 => 'x_tnum',
    );

    $this->assertEquals($exp, (array)$q->getFields());

    $recs = $q->sum();

    $exp = array(
      'origin' => 'UPS print test01',
      'address_id' =>
      array(
        'state_id' =>
        array(
          'country_id' => 'Singapore',
          'id' => 989,
        ),
        'name' => 'Yeo Yen Ping',
        'id' => 1628,
      ),
      'name' => 'PACK1718',
      'x_seller' => 'TEST',
      'x_tnum' => '',
      'carrier_id' => 'R.Mail',
      'x_cust_val' => 32,
      'state' => 'Available',
      'date' => '2011-08-05 13:26:35',
      'id' => 1714,
      'move_lines' =>
      array(
        0 =>
        array(
          'note' => '',
          'id' => 1755,
          'product_id' =>
          array(
            'product_tmpl_id' => 291,
            'default_code' => 'LL-00329',
            'id' => 291,
          ),
          'product_qty' => 1,
        ),
      ),
    );

    $this->assertEquals($exp, $recs[0]);
  }
}

?>