<?php
namespace Gulei\Form\Element;

abstract class Abstract_ButtonLike extends _Abstract
{
  protected function getEntryTemplate()
  {
    $attr = $this->getArchAttributes();
    $tpl = parent::getEntryTemplate();
    $tpl['#value'] = $attr['string'];
    $tpl['#name'] = $attr['name'];
    return $tpl;
  }
}