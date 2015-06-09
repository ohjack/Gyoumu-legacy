<?php
namespace Oerp\Modifier;

class PackHeaderRaw extends \Gulei\Modifier\Pack\_Abstract
{
  protected function init()
  {
    $pack = new \Gulei\Modifier\Pack\HeaderRaw();
    $this->append($pack->getItems());
    $this->append(
      array(
           new TagAddOpColumn(),
           new TagUseParamString(),
      )
    );
  }
}
