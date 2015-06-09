<?php
namespace Gulei\Form\Element;

class Submit extends Abstract_ButtonLike
{
  public function getEntry()
  {
    return $this->getEntryTemplate();
  }
}