<?php
namespace Oerp\Command;

class isBeforeCloseTime extends \Gulei\Command\WithReference\WithTable
{
  function execute($refVar = null){
    list($rec, $name) = $this->ref->vals('rec', 'name');
    $close = date('Y-m-d 00:00:00');
    return strcmp($close, $rec[$name]) > 0;
  }
}