<?php
namespace Listing\Checker;

module_load_include('module', 'amapi');

class Amazon extends _Abstract
{
  private $ArryItems;
  private $no_report = false;

  public function __construct($skus)
  {
    parent::__construct($skus);
    $prov = new \Amapi\Provider\ActiveListing();
    $recs = $prov->getData();

    if($recs){
      $this->ArryItems = new \Gulei\_Array\Basic($recs);
    }
    else{
      $this->no_report = true;
    }
  }

  public function getKey()
  {
    return __CLASS__;
  }

  protected function findItem($sku)
  {
    return $this->ArryItems->exe("Listing\\_Array\\Command\\Amazon\\findItem", $sku);
  }

  public function getHeader()
  {
    return 'Amazon';
  }

  protected function getLink($sku)
  {
    if($this->no_report){
      drupal_set_message('No recent report of Amazon.jp found. Please try again 5 minutes later.', 'error');
      return false;
    }

    $item = $this->findItem($sku);
    return $item
        ? 'http://www.amazon.co.jp/dp/' . $item['商品ID']
        : false;
  }

}