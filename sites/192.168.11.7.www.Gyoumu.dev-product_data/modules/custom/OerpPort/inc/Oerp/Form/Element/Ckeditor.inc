<?php
namespace Oerp\Form\Element;

class Ckeditor extends Text
{
  protected function getEntryTemplate()
  {
    $ele = parent::getEntryTemplate();
    $ele['#attributes']['class'] = 'enable-ckeditor';
    return $ele;
  }
}