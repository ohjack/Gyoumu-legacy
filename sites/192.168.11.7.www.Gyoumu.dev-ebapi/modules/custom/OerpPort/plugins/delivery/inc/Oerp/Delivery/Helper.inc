<?php
namespace Oerp\Delivery;

class Helper
{
  public function getSkuDesc($rec)
  {
    static $mapping;

    if(!$mapping){
      $csvDesc = new \Gulei\Csv\Model(array(
        'filepath' => __DIR__ . '/Helper/SkuDesc.csv',
      ));

      $mapping = array();
      foreach($csvDesc->connect() as $each){
        $pat = '/^' . trim($each['Category']) . '-.*/';
        $mapping[$pat] = $each['Description'];
      }
    }

    $skus = array();
    $qtys = array();

    foreach($rec['move_lines'] as $line){
      $skus[] = $line['product_id']['default_code'];
      $qtys[] = $line['product_qty'];
    }

    $pairs = array();
    foreach($skus as $ind => $sku){
      $desc = preg_replace(array_keys(($mapping)), $mapping, $sku);

      if($desc == $sku){
        $desc = 'Car Spoiler';
      }

      if(isset($pairs[$desc])){
        $pairs[$desc] += $qtys[$ind];
      }
      else{
        $pairs[$desc] = $qtys[$ind];
      }
    }

    foreach($pairs as $desc => &$pair){
      $pair = $desc . '(' . $pair . ')';
    }

    return implode(',', $pairs);
  }

  static public function getCloseTime($type = 'before') {
    $pat = 'Y-m-d H:i:s';

    switch ($type) {
      case 'before':
        return date($pat, mktime(23, 59, 59, date('m'), date('d') - 1));

      case 'after':
        return date($pat, mktime(0, 0, 0));

      default:
        throw new \Exception('Unkown type of Close Time.');
    }
  }
}