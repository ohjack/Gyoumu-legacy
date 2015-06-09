<?php
namespace Gulei\Command\WithReference;

abstract class WithTable extends \Gulei\Command\WithReference\_Abstract
{
  protected function getCommandArgs()
  {
    list($ObjTable, $src) = array(
      $this->ref->val('ObjTable'),
      $this->ref->val('src')
    );
    return $ObjTable->getNodeValues('./arg[not(@name)]', $src, false);
  }

  protected function getCommandNamedArgs()
  {
    list($ObjTable, $src) = array(
      $this->ref->val('ObjTable'),
      $this->ref->val('src')
    );
    return $ObjTable->getNodeValues('./arg[@name]', $src, true);
  }
}