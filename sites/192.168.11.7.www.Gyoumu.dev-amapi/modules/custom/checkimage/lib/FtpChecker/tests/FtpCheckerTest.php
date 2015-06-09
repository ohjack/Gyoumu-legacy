<?php

require_once '../FtpChecker.inc';

class FtpCheckerTest extends PHPUnit_Framework_TestCase
{
  static private $checker;

  protected function create()
  {
    return new FtpChecker('picturemaster.info', 'pictu39', '2xaugadr');
  }

  public static function setUpBeforeClass()
  {
    self::$checker = self::create();
  }

  public function testisLoggedIn(){
    $this->assertEquals(false, self::$checker->isLoggedIn());

    self::$checker->login();
    $this->assertEquals(true, self::$checker->isLoggedIn());
  }

  public function testGetList()
  {
    $list = self::$checker->getList('.');
    $this->assertNotEmpty($list);
  }

  public function testExistFiles()
  {
    $exist = array('.', '..');
    $this->assertEquals(true, self::$checker->existFiles('.', $exist));

    $exist = array('NOTFOUND');
    $this->assertEquals(false, self::$checker->existFiles('.', $exist));
  }
}

?>