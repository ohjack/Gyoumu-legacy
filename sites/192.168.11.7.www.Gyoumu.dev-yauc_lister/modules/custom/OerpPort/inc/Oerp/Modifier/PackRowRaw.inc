<?php
namespace Oerp\Modifier;

class PackRowRaw extends \Gulei\Modifier\Pack\_Abstract
{
  protected function init()
  {
    $pack = new \Gulei\Modifier\Pack\RowRaw();
    $this->append($pack->getItems());
    $this->append(
      array(
        new TagAlterRows(),
        new TagMergeOp(),
      )
    );
  }
}
