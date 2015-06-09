<?php
namespace Gulei\Form\Element;

class Markup extends _Abstract
{
  public function getEntry()
  {
    $tpl = $this->getEntryTemplate();
    $val = $this->xp('./value[1]')->item(0);
    $nodeVal = $val ? $val->nodeValue : '';

    $tpl['#value'] = is_null($default_val = $tpl['#default_value'])
        ? $nodeVal : $default_val;

    return $tpl;
  }
}