<?php
namespace Gulei\Command;

class isAscii extends \Gulei\Command\WithReference\_Abstract
{
  function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');
    return !preg_match("/[^[:alnum:][:punct:][:blank:]]/", $rec[$name]);
  }
}