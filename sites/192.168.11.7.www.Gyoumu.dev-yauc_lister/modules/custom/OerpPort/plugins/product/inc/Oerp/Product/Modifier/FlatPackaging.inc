<?php
namespace Oerp\Product\Modifier;

class FlatPackaging extends \Gulei\Modifier\_Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }

  protected function isRowApplicable($RefVars, $ref)
  {
    return true;
  }

  static private function isPkg($rec, $name)
  {
    return stripos($rec['ul']['name'], $name) === 0;
  }

  static public function sort($a, $b){
    if(self::isPkg($b, 'fedex')){
      return 1;
    }
    if(!self::isPkg($a, 'fedex') && self::isPkg($b, 'original')){
      return 1;
    }
    return -1;
  }

  protected function setFlatPkg(&$row)
  {
    static $ref = null;
    if(is_null($ref)){
      $q = new \Oerp\Query\Basic('product.ul');
      $ref = (array)$q->raw()->promote('name');
     }

    for($c = 0; $c <= 1; $c++){
      if(!$pkg = &$row['packaging'][$c]){
        break;
      }

      $dims = array('length', 'width', 'height');
      $name = $pkg['ul']['name'];
      $ptype = $ref[$name];
      $prefix = 'pkg-' . $c . '-';

      foreach($dims as $dim){
        $val = $pkg[$dim];

        if(!$val && ($dval = $ptype['x_' . $dim])){
          $val = $dval;
        }
        $row[$prefix . $dim] = $val;
      }
      $row[$prefix . 'name'] = $pkg['ul']['name'];
      $row[$prefix . 'weight'] = $pkg['weight'];
    }
    unset($row['packaging']);
  }

  protected function modifyRow($RefVars, $ref)
  {
    $row = &$RefVars->refVar('row');
    usort($row['packaging'], __CLASS__ . '::sort');

    if(count($row['packaging']) < 2){
      $firstPkg = $row['packaging'][0];

      if(self::isPkg($firstPkg, 'fedex')){
        $row['packaging'][] = array();
      }
      else{
        $row['packaging'][] = $firstPkg;
      }
    }

    $this->setFlatPkg($row);
 }
}