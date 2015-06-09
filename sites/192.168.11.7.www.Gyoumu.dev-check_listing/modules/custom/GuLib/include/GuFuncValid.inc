<?php
function GuFunc_dummy($recs, $ind, $rec, $name, $args){
  return true;
}

function GuFunc_isUnique($recs, $ind, $rec, $name, $args){
  $val = $rec[$name];
  
  foreach($recs as $i => $r){
    if(empty($r[$name]) || $i == $ind){
      continue;
    }
    if($r[$name] === $val){
      return false;                  
    }
  }
  return true;
}

function GuFunc_isAscii($recs, $ind, $rec, $name, $args){
  return !preg_match("/[^[:alnum:][:punct:][:blank:]]/", $rec[$name]);  
}

function GuFunc_isEmpty($recs, $ind, $rec, $name, $args){
  return empty($rec[$name]);
}

function GuFunc_pregMatch($recs, $ind, $rec, $name, $args){
  return preg_match($args[0], $rec[$name]);
}

function GuFunc_cmp($val1, $val2, $op){
  switch($op){
    case 'LT':
      return $val1 < $val2;
      
    case 'GT':
      return $val1 > $val2;
      
    case 'LE':
      return $val1 <= $val2;
      
    case 'GE':
      return $val1 >= $val2;
      
    case 'EQ':
      return $val1 == $val2;
      
    case 'NE':
      return $val1 != $val2;
  }
}

function GuFunc_cmpStrLength($recs, $ind, $rec, $name, $args){
  return GuFunc_cmp(mb_strlen($rec[$name]), $args[1], $args[0]);
}
?>
