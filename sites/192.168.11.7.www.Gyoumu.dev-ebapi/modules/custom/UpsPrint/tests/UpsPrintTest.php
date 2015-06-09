<?php

require_once './includes/bootstrap.inc';
$_SERVER['REQUEST_METHOD'] = 'get';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

require_once 'sites/all/modules/custom/UpsPrint/UpsPrint.inc';
require_once 'vfsStream/vfsStream.php';

class UpsPrintControllerTest extends PHPUnit_Framework_TestCase
{
  private $dir;
  private $idSuccess = 'testfile-SP-SinglePC-CA01';
  private $idError = 'testfile-SP-SinglePC-CA01-error';
  private $test_rec;

  protected function createControllerStub($id)
  {
    $stub = $this->getMockBuilder('UpsPrintController')
        ->setConstructorArgs(array($this->dir, $this->dir, $id))
        ->setMethods(array('sendRecord'))
        ->getMock();

    return $stub;
  }

  public function setUp()
  {
    $this->dir = dirname(__FILE__);
    $this->test_rec = array(
      'id' => 1713,
      'tid' => 'PACK1717',
      'recp_email' => '',
      'recp_contact' => 'ACI Express, Inc. Los Angeles Br',
      'recp_phone' => '00',
      'recp_addr1' => '2218 E.Gladwick Street',
      'recp_addr2' => '( #270674194150 )',
      'recp_city' => 'Rancho Dominguez',
      'recp_post_code' => '90220',
      'recp_state' => 'CA',
      'recp_country_code' => 'US',
      'ref_note' => 'UPS print test00',
      'pkg_weight' => 1,
      'custom_value' => 45,
      'service_type' => 'UPS',
      'com_desc1' => 'Car Moulding(1)',
    );
  }

  public function testIsSent()
  {
    $ctrl = $this->createControllerStub($this->idSuccess);
    $this->assertEquals(true, $ctrl->isSent());

    $ctrl = $this->createControllerStub('NOTFOUND');
    $this->assertEquals(false, $ctrl->isSent());
  }

  public function testIsPrint()
  {
    $ctrl = $this->createControllerStub($this->idSuccess);
    $this->assertEquals(true, $ctrl->isPrint());

    $ctrl = $this->createControllerStub('NOPRINT', null);
    $this->assertEquals(false, $ctrl->isPrint());
  }

  public function testGetResponse()
  {
    $ctrl = $this->createControllerStub($this->idSuccess);
    $this->assertNotEmpty($ctrl->getResponse());

    $ctrl = $this->createControllerStub('NORESP', null);
    $this->assertEquals(false, $ctrl->getResponse());
  }

  public function testGetTrackNum()
  {
    $nums = array('1Z95A76A0442437824', '1Z95A76A0441998631');

    $ctrl = $this->createControllerStub($this->idSuccess);
    $this->assertEquals($nums, $ctrl->getTrackNum());

    $ctrl = $this->createControllerStub($this->idError);
    $this->assertEquals(false, $ctrl->getTrackNum());
  }

  public function testGetOutputDate()
  {
    $ctrl = $this->createControllerStub($this->idSuccess);
    $this->assertEquals('20110803', $ctrl->getOutputDate());
  }

  public function testGetLogMessage()
  {
    $ctrl = $this->createControllerStub($this->idError);
    $this->assertNotEmpty($ctrl->getLogMessage());
  }

  public function testGetRecordXml()
  {
    $rec = $this->test_rec;
    $ctrl = $this->createControllerStub($this->test_rec['tid']);
    $exp = \Gulei\File\Helper::getPathContent('module', 'UpsPrint', '/tests/expectedRecord.xml');
    $this->assertXmlStringEqualsXmlString($exp, $ctrl->getRecordXml($rec));
  }

  public function testSendRecordNew()
  {
    vfsStream::setup();
    $dir = vfsStream::url('root');
    $rec = $this->test_rec;

    $ctrl = new UpsPrintController($dir, $dir, $rec['tid']);
    $ctrl->sendRecord($rec);
    $this->assertStringEqualsFile($ctrl->getFilename('xml'), $ctrl->getRecordXml($rec));
  }

  public function testSendRecordReprint()
  {
    $rec = $this->test_rec;

    vfsStream::setup();
    vfsStream::newFile($rec['tid'] . '.xxx')->at(vfsStreamWrapper::getRoot());
    vfsStream::newFile($rec['tid'] . '.Out')->at(vfsStreamWrapper::getRoot());
    $dir = vfsStream::url('root');

    $this->assertFileExists($dir . '/' . $rec['tid'] . '.xxx');
    $this->assertFileExists($dir . '/' . $rec['tid'] . '.Out');

    $ctrl = new UpsPrintController($dir, $dir, $rec['tid']);
    $ctrl->sendRecord($rec);
    $this->assertStringEqualsFile($ctrl->getFilename('xml'), $ctrl->getRecordXml($rec));

    $this->assertFileNotExists(vfsStream::url('root/' . $rec['tid'] . '.xxx'));
    $this->assertFileNotExists(vfsStream::url('root/' . $rec['tid'] . '.Out'));
  }
}