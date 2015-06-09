<?php
namespace Oerp\Product\_Array\Command;

class sortSKU extends \Gulei\_Array\Command\_Abstract
{
  static public function sort(&$recs)
  {
    $cmp_src = '
      $_a = substr($a["default_code"], 3);
      $_b = substr($b["default_code"], 3);
      return strcmp($_a, $_b);
    ';

    $cmp = create_function('$a,$b',$cmp_src);
    usort($recs, $cmp);
  }

  public function execute($args = null)
  {
    $recs = (array)$this->ArryObj;
    self::sort($recs);
    return $recs;
  }
}