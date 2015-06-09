<?php
namespace Gulei\Modifier\Pack;

class HeaderAlt extends _Abstract
{
  protected function init()
  {
    $this->append(
      array(
           new \Gulei\Modifier\Tag\AddCountColumn(),
      )
    );
  }
}