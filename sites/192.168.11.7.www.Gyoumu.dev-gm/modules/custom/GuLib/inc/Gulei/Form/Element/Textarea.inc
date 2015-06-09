<?php
namespace Gulei\Form\Element;

class Textarea extends _Abstract
{
  public function getEntry()
  {
    return $this->getEntryTemplate();
  }
}