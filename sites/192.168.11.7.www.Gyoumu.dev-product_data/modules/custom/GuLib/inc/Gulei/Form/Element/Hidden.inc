<?php
namespace Gulei\Form\Element;


class FormEleHidden extends _Abstract
{
  public function getEntry()
  {
    $attr = $this->getArchAttributes();
    $tpl = $this->getEntryTemplate();
    $tpl['#value'] = $attr['value'];
    return $tpl;
  }
}