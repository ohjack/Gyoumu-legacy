<?php

function OerpFunc_SearchOpMap($str){
  return str_replace(
    array('LT', 'LE', 'GT', 'GE', 'EQ'),
    array('<', '<=', '>', '>=', '='),
    $str
  );
}

function OerpFunc_isValidSku($recs, $ind, $rec, $name, $args){
  return !!preg_match('/^[A-Z]{2}-\d{5}$/', $rec[$name]);
}

?>