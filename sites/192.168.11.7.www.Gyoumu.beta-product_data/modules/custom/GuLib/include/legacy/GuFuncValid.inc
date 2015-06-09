<?php
function GuFunc_dummy($recs, $ind, $rec, $name, $args){
  return true;
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
?>
