<?php
namespace Gulei\Form\Element;

class Textfield extends _Abstract
{
  public function getEntry()
  {
    $tpl = $this->getEntryTemplate();
    if (!isset($this->param['size'])) {
      $tpl['#size'] = '';
    }
    return $tpl;
  }
}