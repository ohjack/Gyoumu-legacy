<?php
namespace Gulei\File\Finder;

class Local extends _Abstract
{
  function search($pat, $TOSTR = false, $flag = 0){
    $pat = '/' . ltrim($pat, '/');
    $rs = glob($this->base . $pat, $flag);

    if($TOSTR){
      return implode("\n", $rs);
    }else{
      return $rs;
    }
  }

  function getList($pat = null, $TOSTR = false, $flag = 0){
    $pat = is_null($pat) ? '*' : $pat;
    return $this->search($pat, $TOSTR, $flag);
  }
}