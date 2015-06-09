<?php
namespace Gulei\Command;

class explode extends \Gulei\Command\WithReference\WithTable
{
  function execute($refVar = null)
  {
    $args = $this->getCommandArgs();
    list($rec, $name) = $this->ref->vals('rec', 'name');

    $toks = explode($args[0], $rec[$name]);
    return $toks[$args[1]];
  }
}