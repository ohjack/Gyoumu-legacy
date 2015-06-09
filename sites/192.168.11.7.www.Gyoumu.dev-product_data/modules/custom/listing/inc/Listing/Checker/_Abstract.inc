<?php
namespace Listing\Checker;

abstract class _Abstract
{
  abstract protected function findItem($sku);
  abstract public function getHeader();
  abstract protected function getLink($sku);
  abstract public function getKey();

  protected $skus;

  public function __construct($sku_src)
  {
    $this->setSkus($sku_src);
  }

  private function setSkus($sku_src)
  {
    $skus = explode("\n", $sku_src);
    $skus = array_map('trim', $skus);
    $skus = array_map('strtoupper', $skus);
    $skus = array_filter($skus);
    $this->skus = $skus;
  }

  public function getSkus()
  {
    return $this->skus;
  }

  protected function getLinkOpt()
  {
    return array('attributes' => array('target' => '_blank'));
  }

  protected function getErrorMsg()
  {
    return '<span class="error">N/A</span>';
  }

  protected function getLinkSrc($links)
  {
    $items = array();
    if(!is_array($links)){
      $links = array($links);
    }

    foreach($links as $link){
      $items[] = l('Link', $link, $this->getLinkOpt());
    }
    return '<div>' . implode('<br/>', $items) . '</div>';
  }

  public function getResult($sku)
  {
    return ($link = $this->getLink($sku))
        ? $this->getLinkSrc($link)
        : $this->getErrorMsg();
  }
}