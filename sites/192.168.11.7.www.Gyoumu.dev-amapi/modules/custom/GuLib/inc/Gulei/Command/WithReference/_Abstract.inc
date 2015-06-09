<?php
namespace Gulei\Command\WithReference;

abstract class _Abstract extends \Gulei\Reference\Wrapper\_Abstract implements \Gulei\Command\_Interface
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