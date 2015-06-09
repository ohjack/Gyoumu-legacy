<?php
namespace Oerp\Product\Modifier;

class BestPrice extends \Gulei\Modifier\_Abstract
{
  static private $prev_supp;

  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  private function getBestPricelist($pricelist, $rec)
  {
    $sup_key = 'product_tmpl_id.seller_ids.name.name';
    $keys = array(
      'qty' => 'product_tmpl_id.seller_ids.pricelist_ids.min_quantity',
      'price' => 'product_tmpl_id.seller_ids.pricelist_ids.price'
    );

    if(!is_null($rec[$sup_key])){
      self::$prev_supp = $rec[$sup_key];
    }

    if(is_null($pricelist[$keys['price']])
       || $rec[$keys['price']] < $pricelist[$keys['price']]){

      foreach($keys as $key){
        $pricelist[$key] = $rec[$key];
      }
      $pricelist[$sup_key] = self::$prev_supp;
    }
    return $pricelist;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $recs = &$RefVars->refVar('rows');
    $anchor = null;
    $pricelist = array();

    $count = count($recs);

    for($c = 0; $c < $count; $c++){
      if(isset($recs[$c]['id'])){
        if($recs[$c]['id'] && $recs[$c]['default_code']){
          $anchor = &$recs[$c];
          $pricelist = $this->getBestPricelist(array(), $recs[$c]);
        }
        else{
          unset($recs[$c]);
        }
      }
      else{
        $pricelist = $this->getBestPricelist($pricelist, $recs[$c]);
        $anchor = array_merge($anchor, $pricelist);
        unset($recs[$c]);
      }
    }

    $cmp_src = '
      $_a = substr($a["default_code"], 3);
      $_b = substr($b["default_code"], 3);
      return strcmp($_a, $_b);
    ';

    $cmp = create_function('$a,$b',$cmp_src);
    usort($recs, $cmp);
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}