<?php
namespace Listing\Checker;

module_load_include('module', 'yaucapi');

class YahooJp extends _Abstract
{
  public function getKey()
  {
    return __CLASS__;
  }

  public function getHeader()
  {
    return 'YahooJp';
  }

  public function findItem($sku)
  {
    $q = new \Yauc\Query\Item();
    return $q->query($sku);
  }

  protected function getLink($sku)
  {
    $items = $this->findItem($sku);

    $links = array();
    foreach($items as $item){
      $links[] = $item->auc_url;
    }
    return $links ? $links : false;
  }
}