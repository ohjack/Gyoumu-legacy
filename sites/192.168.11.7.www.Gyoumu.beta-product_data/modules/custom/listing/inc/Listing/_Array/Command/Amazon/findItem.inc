<?php
namespace Listing\_Array\Command\Amazon;

class findItem extends \Gulei\_Array\Command\_Abstract
{
  public function execute($sku = null)
  {
    foreach($this->ArryObj as $rec) {
      if($sku == strtoupper($rec['出品者SKU'])){
        return $rec;
      }
    }
    return false;
  }
}