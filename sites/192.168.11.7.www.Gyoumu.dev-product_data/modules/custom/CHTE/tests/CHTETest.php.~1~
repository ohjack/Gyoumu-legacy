<?php

require_once './includes/bootstrap.inc';
$_SERVER['REQUEST_METHOD'] = 'get';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

class CHTETest extends PHPUnit_Framework_TestCase
{
  public function setUp()
  {
    $this->theme_id = 333;
    $this->theme_src = node_load($this->theme_id)->body;
    $this->csv_id = 335;
    $this->csv_src = node_load($this->csv_id)->body;

    $this->fp = '/home/gulei/www/Gyoumu/sites/all/modules/custom/CHTE/tests/SampleInput.csv';

    $this->ctrl = new ChteTemplate(
      $this->theme_id,
      $this->csv_id,
      $this->fp
    );
  }

  public function testInit()
  {
    $this->assertNotEmpty($this->theme_src);
    $this->assertNotEmpty($this->csv_src);
    $this->assertFileExists($this->fp);
  }

  public function testInput()
  {
    $this->objInputCsv = new GuCsv(array('filepath' => $this->fp));
    $this->inputRecs = $this->ctrl->getInputRecs();

    $expected = $this->objInputCsv->connect();

    $this->assertNotEmpty($expected);
    $this->assertEquals($expected, $this->inputRecs);
  }

  public function testOutputTplRecs()
  {
    $this->objOutputCsv = new GuCsv(array('source' => $this->csv_src));
    $this->outputTplRecs = $this->ctrl->getOutputTemplateRecs();

    $expected = $this->objOutputCsv->connect();

    $this->assertNotEmpty($expected);
    $this->assertEquals($expected, $this->outputTplRecs);
  }
/*
  public function testOutputTplKeys()
  {
    $this->keys = array(
      0 => 'clabel',
      1 => 'POS_lv1',
      2 => 'menu_lv1',
      3 => 'POS_lv2',
      4 => 'menu_lv2',
      5 => 'POS_lv3',
      6 => 'menu_lv3',
      7 => 'POS_lv4',
      8 => 'menu_lv4',
      9 => 'POS_lv5',
      10 => 'menu_lv5',
      11 => 'title',
      12 => 'HTML',
      13 => 'format1',
      14 => 'format2',
      15 => 'price',
      16 => 'qty',
      17 => 'duration',
      18 => 'end_time',
      19 => 'free_shipping',
      20 => 'shipping',
      21 => 'relist',
    );
    $this->assertEquals($this->keys, $this->ctrl->getHeaders());
  }

  public function testOutputDefaults()
  {
    $defaults = array(
      'clabel' => '',
      'POS_lv1' => '2',
      'menu_lv1' => 'パーツ',
      'POS_lv2' => '1',
      'menu_lv2' => '内装品',
      'POS_lv3' => '1',
      'menu_lv3' => '自動車メーカー別',
      'POS_lv4' => '1',
      'menu_lv4' => 'BMW用',
      'POS_lv5' => '1',
      'menu_lv5' => 'ON',
      'title' => '',
      'HTML' => '',
      'format1' => 'salesmode_buynow',
      'format2' => 'auc_BidOrBuyPrice',
      'price' => '',
      'qty' => '',
      'duration' => '7',
      'end_time' => '20',
      'free_shipping' => 'auc_shipping_seller',
      'shipping' => '0',
      'relist' => '0',
    );

    $this->defaults = $this->ctrl->getDefaults();
    $this->assertEquals($defaults, $this->defaults);
  }

  public function testGetRecords()
  {
    $rec_0 = array(
      'clabel' => 'DR-00035',
      'clabel_LOWERCASE' => 'dr-00035',
      'clabel_UPPERCASE' => 'DR-00035',
      'POS_lv1' => '2',
      'POS_lv1_LOWERCASE' => '2',
      'POS_lv1_UPPERCASE' => '2',
      'menu_lv1' => 'パーツ',
      'menu_lv1_LOWERCASE' => 'パーツ',
      'menu_lv1_UPPERCASE' => 'パーツ',
      'POS_lv2' => '1',
      'POS_lv2_LOWERCASE' => '1',
      'POS_lv2_UPPERCASE' => '1',
      'menu_lv2' => '内装品',
      'menu_lv2_LOWERCASE' => '内装品',
      'menu_lv2_UPPERCASE' => '内装品',
      'POS_lv3' => '1',
      'POS_lv3_LOWERCASE' => '1',
      'POS_lv3_UPPERCASE' => '1',
      'menu_lv3' => '自動車メーカー別',
      'menu_lv3_LOWERCASE' => '自動車メーカー別',
      'menu_lv3_UPPERCASE' => '自動車メーカー別',
      'POS_lv4' => '1',
      'POS_lv4_LOWERCASE' => '1',
      'POS_lv4_UPPERCASE' => '1',
      'menu_lv4' => 'BMW用',
      'menu_lv4_LOWERCASE' => 'bmw用',
      'menu_lv4_UPPERCASE' => 'BMW用',
      'POS_lv5' => '1',
      'POS_lv5_LOWERCASE' => '1',
      'POS_lv5_UPPERCASE' => '1',
      'menu_lv5' => 'ON',
      'menu_lv5_LOWERCASE' => 'on',
      'menu_lv5_UPPERCASE' => 'ON',
      'title' => '即決/BMW E34 E32メッキクロームゲージメーターリング DR-00035',
      'title_LOWERCASE' => '即決/bmw e34 e32メッキクロームゲージメーターリング dr-00035',
      'title_UPPERCASE' => '即決/BMW E34 E32メッキクロームゲージメーターリング DR-00035',
      'HTML' => '',
      'HTML_LOWERCASE' => '',
      'HTML_UPPERCASE' => '',
      'format1' => 'salesmode_buynow',
      'format1_LOWERCASE' => 'salesmode_buynow',
      'format1_UPPERCASE' => 'SALESMODE_BUYNOW',
      'format2' => 'auc_BidOrBuyPrice',
      'format2_LOWERCASE' => 'auc_bidorbuyprice',
      'format2_UPPERCASE' => 'AUC_BIDORBUYPRICE',
      'price' => '3200',
      'price_LOWERCASE' => '3200',
      'price_UPPERCASE' => '3200',
      'qty' => '1',
      'qty_LOWERCASE' => '1',
      'qty_UPPERCASE' => '1',
      'duration' => '7',
      'duration_LOWERCASE' => '7',
      'duration_UPPERCASE' => '7',
      'end_time' => '20',
      'end_time_LOWERCASE' => '20',
      'end_time_UPPERCASE' => '20',
      'free_shipping' => 'auc_shipping_seller',
      'free_shipping_LOWERCASE' => 'auc_shipping_seller',
      'free_shipping_UPPERCASE' => 'AUC_SHIPPING_SELLER',
      'shipping' => '0',
      'shipping_LOWERCASE' => '0',
      'shipping_UPPERCASE' => '0',
      'relist' => '0',
      'relist_LOWERCASE' => '0',
      'relist_UPPERCASE' => '0',
      'head' => '即決/BMW E34 E32メッキクロームゲージメーターリング',
      'head_LOWERCASE' => '即決/bmw e34 e32メッキクロームゲージメーターリング',
      'head_UPPERCASE' => '即決/BMW E34 E32メッキクロームゲージメーターリング',
      'model' => 'BMW E34
BMW E32',
      'model_LOWERCASE' => 'bmw e34
bmw e32',
      'model_UPPERCASE' => 'BMW E34
BMW E32',
      'desc' => 'DR-00035 DESC1
DR-00035 DESC2',
      'desc_LOWERCASE' => 'dr-00035 desc1
dr-00035 desc2',
      'desc_UPPERCASE' => 'DR-00035 DESC1
DR-00035 DESC2',
      'model_LINES' =>
      array(
        0 => 'BMW E34',
        1 => 'BMW E32',
      ),
      'model_LOWERCASE_LINES' =>
      array(
        0 => 'bmw e34',
        1 => 'bmw e32',
      ),
      'model_UPPERCASE_LINES' =>
      array(
        0 => 'BMW E34',
        1 => 'BMW E32',
      ),
      'desc_LINES' =>
      array(
        0 => 'DR-00035 DESC1',
        1 => 'DR-00035 DESC2',
      ),
      'desc_LOWERCASE_LINES' =>
      array(
        0 => 'dr-00035 desc1',
        1 => 'dr-00035 desc2',
      ),
      'desc_UPPERCASE_LINES' =>
      array(
        0 => 'DR-00035 DESC1',
        1 => 'DR-00035 DESC2',
      ),
    );

    $this->records = $this->ctrl->getRecords();
    $this->assertEquals($rec_0, $this->records[0]);
  }

  public function testPhptalInit()
  {
    $this->phptal = $this->ctrl->getPhptal();
    $this->assertNotEmpty($this->phptal);
  }

  public function testGetAppliedRecs()
  {
    $this->appliedRecs = $this->ctrl->getAppliedRecs();
    $this->assertNotEmpty($this->appliedRecs[0]['HTML']);
  }

  public function testDumpAppliedCsv()
  {
    $this->appliedCsv = $this->ctrl->getAppliedCsv();
    $this->assertNotEmpty($this->appliedCsv);
  }
*/
}

?>