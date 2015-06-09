<?php
namespace Gulei\Command;

class Phptal extends \Gulei\Command\WithReference\WithTable {
  public function execute($ref_var = null) {
    list($rec) = $this->ref->vals('rec');

    $args = $this->getCommandArgs();
    $tpl = new \PHPTAL(array_shift($args));

    foreach ($args as $arg) {
      $tpl->$arg = $rec[$arg];
    }
    return $tpl->execute();
  }
}