<?php
/**
 * Created by JetBrains PhpStorm.
 * User: gulei
 * Date: 7/21/11
 * Time: 4:44 PM
 * To change this template use File | Settings | File Templates.
 */

include_once('YaucDriver.inc');

class YaucDriverTest extends PHPUnit_Framework_TestCase
{
  static protected $driver;
  static protected $username = 'yjqnk880';
  static protected $password = 'banribanri';

  protected $testAuctionLink = 'http://page4.auctions.yahoo.co.jp/jp/auction/d118985648';

  public static function setUpBeforeClass()
  {
    self::$driver = new YaucDriver(self::$username, self::$password);
    self::$driver->start();
  }

  public static function tearDownAfterClass()
  {
    //    self::$driver->stop();
  }

  public function testInit()
  {
    $this->assertNotEmpty(self::$driver);
  }

  public function testGotoIndex()
  {
    self::$driver->gotoIndex();
    $isIndex = self::$driver->isTextPresent('商品を検索する');
    $this->assertEquals(true, $isIndex);
    //    $title = self::$driver->getTitle();
    //    $this->assertEquals('Yahoo!オークション', $title);
  }

  public function testGetUser()
  {
    $user = self::$driver->getUser();
    $this->assertNotEmpty($user);
  }

  public function testLogin()
  {
    self::$driver->login();
    $this->assertEquals(self::$username, self::$driver->getUser());
  }

  public function testGotoAuctionByLink()
  {
    self::$driver->gotoAuctionByLink($this->testAuctionLink);
    $isPresent = self::$driver->isTextPresent('商品の情報');
    $this->assertEquals(true, $isPresent);
  }

  public function testGotoAuctionById()
  {
    self::$driver->gotoAuctionById('d118985648');
    $isPresent = self::$driver->isTextPresent('商品の情報');
    $this->assertEquals(true, $isPresent);
  }

  public function testEndAuctionByLink()
  {
    self::$driver->endAuctionByLink('http://page6.auctions.yahoo.co.jp/jp/auction/f105934587');
    $isPresent = self::$driver->isTextPresent('オークションは終了しましたが、落札者はいません。この商品を再出品することができます。');
    $this->assertEquals(true, $isPresent);
  }
/*
  public function testEndAuctionById()
  {
    self::$driver->endAuctionById('h155268811');
    $isPresent = self::$driver->isTextPresent('オークションは終了しましたが、落札者はいません。この商品を再出品することができます。');
    $this->assertEquals(true, $isPresent);
  }
*/
/*
  public function testRelistAuction()
  {
    self::$driver->relistAuction($this->testAuctionLink);
    $isPresent = self::$driver->isTextPresent('以下の商品の出品手続きが完了しました。ご利用ありがとうございました。');
    $this->assertEquals(true, $isPresent);
  }
*/
}