<?php
namespace Gulei\Form\Element;

class Select extends Abstract_tWithOption
{
  public function getEntry()
  {
    return $this->getEntryTemplate();
  }
}