<?php
namespace Gulei\Command;

class isEmpty extends WithReference\_Abstract
{
  function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');
    return empty($rec[$name]);
  }
}