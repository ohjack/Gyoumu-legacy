<?php

require_once './includes/bootstrap.inc';
$_SERVER['REQUEST_METHOD'] = 'get';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

class OerpArchQueryTest extends PHPUnit_Framework_TestCase
{
  protected $arch;
  protected $raw;
  protected $fields;

  protected function create()
  {
    return new OerpArchQuery($this->arch);
  }

  public function setUp()
  {
    $this->arch = <<<XML
<form model="stock.picking">
  <param name="oerp:ids">[36]</param>
  <field name="name"/>
  <field name="move_lines"/>
</form>
XML;

    $this->raw = array(
      0 =>
      array(
        'move_lines' =>
        array(
          0 => 36,
        ),
        'name' => 'PACK36',
        'id' => 36,
      ),
    );

    $this->fields = array(
      'name', 'move_lines'
    );
  }

  public function testCreate()
  {
    $this->assertNotEmpty($this->create());
  }

  public function testGetParam()
  {
    $exp = array(
      'oerp:ids' =>
      array(
        0 => 36,
      ),
      'oerp:model' => 'stock.picking',
      'oerp:criteria' => NULL,
      'oerp:fields' =>
      array(
        0 => 'name',
        1 => 'move_lines',
      ),
    );

    $q = $this->create();
    $this->assertEquals($exp, $q->getParam());
  }

  public function testGetFields()
  {
    $q = $this->create();
    $this->assertEquals($this->fields, $q->getFields());
  }

  public function testRaw()
  {
    $q = $this->create();
    $this->assertEquals($this->raw, (array)$q->raw());
  }
}

?>