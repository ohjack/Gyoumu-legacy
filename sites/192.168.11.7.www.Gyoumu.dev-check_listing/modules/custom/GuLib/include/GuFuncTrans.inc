<?php 
function GuFunc_unaccent($recs, $ind, $rec, $name, $args){
  $h = new GuTextHelper();
  $val = $rec[$name];
  $newVal = $h->asciiize($val);
  
  return ($val === $newVal) ? false : $newVal;
}

function GuFunc_fixSpainAddr($recs, $ind, $rec, $name, $args){
  $rs = preg_replace(
    array(
      '/nº/',
      '/(\d*1)(º|ª)/',
      '/(\d*2)(º|ª)/',
      '/(\d*3)(º|ª)/',
      '/([4-9]|\d{2,})(º|ª)/',
    ),
    array(
      'No.',
      '\1st',
      '\1nd',
      '\1rd',
      '\1th',
    ),
    $rec[$name]
  );
  return $rs;
}

function GuFunc_explode($recs, $ind, $rec, $name, $args){
 $toks = explode($args[0], $rec[$name]);
  return $toks[$args[1]];
}

function GuFunc_appendChkCode($recs, $ind, $rec, $name, $args){
  $val = $rec[$name];
  preg_match("/[^\d]*(\d*)/", $val, $m);
  return $val . chr((int)$m[1] % 26 + 65);
}

function GuFunc_phptal($recs, $ind, $rec, $name, $args){
  $tpl = new PHPTAL(array_shift($args));
  
  foreach($args as $arg){
    $tpl->$arg = $rec[$arg];
  }
  $src = $tpl->execute();
  return $src;
}

?>