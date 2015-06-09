<?php
namespace Oerp\_Array\Command;

class sortSkuByKey extends \Gulei\_Array\Command\_Abstract
{
  static public function sort(&$recs)
  {
    $func = create_function('$a,$b', '
      return substr($a, 3) > substr($b, 3);
    ');
    uksort($recs, $func);
  }

  public function execute($refVar = null)
  {
    $recs = (array)$this->ArryObj;
    self::sort($recs);
    return $recs;
  }
}