<?php
use Gulei as G;
require_once __DIR__ . '/../include/GuTable.inc';
require_once __DIR__ . '/inc/func.inc';

class GuArchTableTest extends PHPUnit_Framework_TestCase
{
  public function testGetHeaders()
  {
    $arch = <<<XML
<tree>
  <field name="f1" string="Field 1">
    <param name="hide">1</param>
  </field>
  <field name="f2" string="Field 2"/>
</tree>
XML;

    $t = new GuArchTable($arch, array());
    $exp = array(
      'f1' => array(
        'data' => 'Field 1',
        'gu_name' => 'f1',
        'rowspan' => 1,
        'style' => 'display:none;',
      ),
      'f2' => array(
        'data' => 'Field 2',
        'gu_name' => 'f2',
        'rowspan' => 1,
      ),
    );

    $this->assertEquals($exp, $t->getHeaders());
  }

  public function testGetRows()
  {
    $arch = <<<XML
<tree>
  <field name="f1">
    <param name="hide">1</param>
  </field>
</tree>
XML;

    $recs = array(
      array('f1' => '1')
    );

    $exp = array(
      array(
        'data' => array(
          'f1' => array(
            'data' => '1',
            'gu_name' => 'f1',
            'rowspan' => 1,
            'style' => 'display:none;',
          ),
        ),
        'class' => 'merge-odd',
      )
    );

    $t = new GuArchTable($arch, $recs);
    $this->assertEquals($exp, $t->getRows($recs));
  }

  public function testGetRecordsCount()
  {
    $arch = <<<XML
<tree>
  <field name="f1"/>
</tree>
XML;

    $recs = array(
      array('f1' => '1'),
      array('f1' => '2')
    );

    $t = new GuArchTable($arch, new G\ProviderLambda($recs));
    $this->assertEquals(2, $t->getRecordsCount());
  }

  public function testGetRecords()
  {
    $arch = <<<XML
<tree>
  <field name="f1"/>
</tree>
XML;

    $recs = array(
      array('f1' => 1),
      array('f1' => 2),
    );

    $t = new GuArchTable($arch, new G\ProviderLambda($recs));
    $this->assertEquals($recs, $t->getRecords());
  }

  private function _testCommand($type, $param)
  {
    $arch = file_get_contents(__DIR__ . '/xml/arch' . $type . 'Test.xml');
    $srcRecs = file_get_contents(__DIR__ . '/eval/src' . $type . 'Recs.txt');
    $srcRows = file_get_contents(__DIR__ . '/eval/src' . $type . 'Rows.txt');
    $srcTheme = file_get_contents(__DIR__ . '/eval/src' . $type . 'Theme.txt');

    $this->assertNotEmpty($arch);
    $recs = eval($srcRecs);
    $this->assertNotEmpty($recs);
    $rows = eval($srcRows);
    $this->assertNotEmpty($rows);
    $this->assertNotEmpty($srcTheme);

    $t = new GuArchTable($arch, new G\ProviderLambda($recs), $param);
    $this->assertEquals($recs, $t->getRecords());
    $this->assertEquals($rows, $t->getRowsAltered());
    $this->assertXmlStringEqualsXmlString($srcTheme, $t->theme());
  }

  public function testTranslate()
  {
    $param = array(
      'mark_translated' => false,
      'mark_invalid' => true,
      'mark_notice' => true,
      'mark_missing' => true,
      'copy_method' => 'ditto',
      'nl2br' => true,
      'sticky_header' => true,
      'pager' => true,
      'items_per_page' => 80,
      'search' => NULL,
    );

    $this->_testCommand('Translate', $param);
  }

  public function testValidate()
  {
    $param = array(
      'mark_translated' => true,
      'mark_invalid' => true,
      'mark_notice' => true,
      'mark_missing' => true,
      'copy_method' => 'ditto',
      'nl2br' => true,
      'sticky_header' => true,
      'count' => true,
      'filepath' => '/tmp/temporders_19.csv',
    );

    $this->_testCommand('Validate', $param);
  }

  public function testFilter()
  {
    $param = array(
      'merge' => '1',
      'copy_method' => 'ditto',
      'mark_invalid' => true,
      'class' => 'print',
      'count' => '1',
      'extra_info' => '1',
      'sticky_header' => false,
      'oerp:cri_ex' => NULL,
    );

    $this->_testCommand('Filter', $param);
  }
}