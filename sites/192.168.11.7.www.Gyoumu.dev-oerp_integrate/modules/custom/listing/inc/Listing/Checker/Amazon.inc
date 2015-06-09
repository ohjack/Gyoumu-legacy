<?php
namespace Listing\Checker;

module_load_include('module', 'amapi');

class Amazon extends _Abstract
{
  private $ArryItems;

  public function __construct($skus)
  {
    parent::__construct($skus);
    $prov = new \Amapi\Provider\ActiveListing();
    $this->ArryItems = new \Gulei\_Array\Basic($prov->getData());
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
    $item = $this->findItem($sku);
    return $item
        ? 'http://www.amazon.co.jp/dp/' . $item['商品ID']
        : false;
  }

}