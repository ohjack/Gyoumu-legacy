<?php
require_once __DIR__  . '/../fedfetch.inc';

class OcsRecFetcherTest extends PHPUnit_Framework_TestCase
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
}