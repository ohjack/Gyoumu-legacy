<?php
namespace Gulei\Form\Element;

class Checkboxes extends Abstract_tWithOption
{
  public function getEntry()
  {
    return $this->getEntryTemplate();
  }
}