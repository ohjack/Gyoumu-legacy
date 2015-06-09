<?php
namespace Oerp\Command;

class isValidSku extends \Gulei\Command\WithReference\_Abstract
{
  function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');
    return !!preg_match('/^[A-Z]{2}-\d{5}$/', $rec[$name]);
  }
}