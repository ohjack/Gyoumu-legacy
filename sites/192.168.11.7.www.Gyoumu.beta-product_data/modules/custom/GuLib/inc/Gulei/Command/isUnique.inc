<?php
namespace Gulei\Command;

class isUnique extends WithReference\_Abstract
{
  function execute($refVar = null){
    list($recs, $rec, $name, $ind) = $this->ref->vals('recs', 'rec', 'name', 'ind');
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
}