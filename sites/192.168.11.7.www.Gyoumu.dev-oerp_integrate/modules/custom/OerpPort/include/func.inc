<?php

function OerpFunc_existValue($recs, $ind, $rec, $name, $args){
  $cri = json_decode($args[1]);
  
  $cri[] = array(
    $args[2], '=', $rec[$name]
  );
  
  $q = new \Oerp\Query\Basic($args[0], $cri, array('id'));
  $resp = (array)$q->raw();
  
  return !empty($resp);
}

function OerpFunc_SearchOpMap($str){
  return str_replace(
    array('LT', 'LE', 'GT', 'GE', 'EQ'),
    array('<', '<=', '>', '>=', '='),
    $str
  );
}

function OerpFunc_isEmptyString($recs, $ind, $rec, $name, $args){
  if(is_null($rec[$name]) || $rec[$name] === false){
    return true;
  }
  else if(strlen(trim($rec[$name])) == 0){
    return true;
  }
  return false;
}

function OerpFunc_isValidSku($recs, $ind, $rec, $name, $args){
  return !!preg_match('/^[A-Z]{2}-\d{5}$/', $rec[$name]);
}

?>