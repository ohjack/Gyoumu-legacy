<?php
namespace Gulei\Command;

class PregMatch extends WithReference\WithTable {
  function execute($ref_var = NULL) {
    list($rec, $name) = $this->ref->vals('rec', 'name');
    $args = $this->getCommandArgs();
    return preg_match($args[0], $rec[$name]);
  }
}