<?php
namespace Oerp\Command;

class getFedexPrintDesc extends \Gulei\Command\WithReference\WithTable
{
  function execute($refVar = null)
  {
    static $mapping;

    list($rec, $name) = $this->ref->vals('rec', 'name');

    if(!$mapping){
      $csvDesc = new \Gulei\Csv\Model(array(
        'filepath' => drupal_get_path('module', 'oerp_delivery') . '/FedexPrintDesc.csv',
      ));

      $mapping = array();
      foreach($csvDesc->connect() as $each){
        $pat = '/^' . trim($each['Category']) . '-.*/';
        $mapping[$pat] = $each['Description'];
      }
    }

    $skus = explode("\n", $rec["move_lines.product_id.default_code"]);
    $qtys = explode("\n", $rec["move_lines.product_qty"]);

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
}