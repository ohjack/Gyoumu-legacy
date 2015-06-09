<?php
require_once __DIR__  . '/../fedfetch.inc';

class ExportRecFetcherTest extends PHPUnit_Framework_TestCase
{
  private static $f;
  private static $testIds;

  public static function setUpBeforeClass()
  {
    self::$f = new ExportRecFetcherFedex();
  }

  public static function tearDownAfterClass()
  {
    self::$f = null;
  }

  public function testInit()
  {
    $this->assertNotEmpty(self::$f);
    $this->assertContains('href="FedEX_login.jsp"', self::$f->getLoginMessage());
  }

  public function testAdaptDate()
  {
    $exp = array(
      'year' => '100',
      'month' => '01',
      'day' => '01',
      'date' => '20110101',
    );

    $this->assertEquals($exp, self::$f->adaptDate(2011, 1, 1));
    $this->assertEquals($exp, self::$f->adaptDate(100, 1, 1));

    $exp = array(
      'year' => '100',
      'month' => '10',
      'day' => '10',
      'date' => '20111010',
    );

    $this->assertEquals($exp, self::$f->adaptDate(2011, 10, 10));

    $exp = array(
      'year' => '100',
      'month' => '10',
      'day' => '__',
      'date' => '201110__',
    );

    $this->assertEquals($exp, self::$f->adaptDate(2011, 10, null));
  }

  public function testGetDailyRecIds()
  {
    $ids = self::$f->getDailyRecIds(2011, 9, 26);
    $this->assertNotEmpty($ids);
    $this->assertEquals('110926192718131_CF  00550YN602', $ids[0]);
    self::$testIds = array($ids[0]);
  }

  public function testGetRaw()
  {
    $raw = self::$f->getRaw(self::$testIds);
    $this->assertNotEmpty($raw);
  }
}