<?php
namespace Gulei\Modifier\Pack;

class HeaderRaw extends _Abstract
{
  protected function init()
  {
    $this->append(
      array(
           new \Gulei\Modifier\Tagize(),
           new \Gulei\Modifier\Tag\Hide(),
      )
    );
  }
}