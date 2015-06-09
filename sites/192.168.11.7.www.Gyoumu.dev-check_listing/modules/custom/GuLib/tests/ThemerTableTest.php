<?php
use Gulei as G;
require 'phpunit_setup_simple.inc';
require_once __DIR__ . '/../include/themer.inc';

class ThemerTableTest extends PHPUnit_Framework_TestCase
{
  public function testInit()
  {
    $ProcHds = new GuProcessor(array('isHds' => true));
    $ProcRecs = new GuProcessor(array('isRecs' => true));

    $themer = new G\ThemerTable($ProcHds, $ProcRecs);
    $this->assertEquals($ProcHds, $themer->getProcHds());
    $this->assertEquals($ProcRecs, $themer->getProcRecs());
  }

  public function testTheme()
  {
    $ProcHds = new GuProcessor();
    $ProcHds->setDataProvider(
      new G\ProviderLambda(array('f1'))
    );

    $ProcRecs = new GuProcessor();
    $ProcRecs->setDataProvider(
      new G\ProviderLambda(array(array('f1' => 'Field 1')))
    );

    $exp = <<<XML
<table class="sticky-enabled">
 <thead><tr><th>f1</th> </tr></thead>
<tbody>
 <tr class="odd"><td gu_name="f1" rowspan="1">Field 1</td> </tr>
</tbody>
</table>
XML;

    $themer = new G\ThemerTable($ProcHds, $ProcRecs);
    $this->assertXmlStringEqualsXmlString($exp, $themer->theme());
  }
}