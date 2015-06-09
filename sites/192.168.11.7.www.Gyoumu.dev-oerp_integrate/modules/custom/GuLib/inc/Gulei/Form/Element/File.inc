<?php
namespace Gulei\Form\Element;

class FormEleFile extends _Abstract
{
  public function getEntry()
  {
    $tpl = $this->getEntryTemplate();
    $tpl['#size'] = '';
    return $tpl;
  }
}