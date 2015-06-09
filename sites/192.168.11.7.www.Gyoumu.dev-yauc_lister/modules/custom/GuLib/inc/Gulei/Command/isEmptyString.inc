<?php
namespace Gulei\Command;

class isEmptyString extends WithReference\_Abstract
{
  function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');

    if(is_null($rec[$name]) || $rec[$name] === false){
      return true;
    }
    else if(strlen(trim($rec[$name])) == 0){
      return true;
    }
    return false;
  }
}