<?php

class GuFileFinder{
  private $base;
  
  function __construct($base){
    $this->base = rtrim($base, '/');
  }
  
  function getBase(){
    return $this->base;
  }
  
  function search($pat, $TOSTR = false, $flag = 0){
    $pat = '/' . ltrim($pat, '/');
    $rs = glob($this->base . $pat, 0);
    
    if($TOSTR){
      return implode("\n", $rs);
    }else{
      return $rs;
    }
  }
  
  function pregSearch($pat, $ref = null){
    $list = $this->getList();
    
    $found = array();
    foreach($list as $key => $file){
      if(preg_match($pat, $file, $m)){
        if($ref){
          $found[$m[$ref]] = $file;
        }
        else{
          $found[] = $file;
        }
      }
    }
    
    return $found;
  }
  
  function getList($pat = null, $TOSTR = false, $flag = 0){
    $pat = is_null($pat) ? '*' : $pat;
    return $this->search($pat, $TOSTR, $flag);
  }
}
?>	
