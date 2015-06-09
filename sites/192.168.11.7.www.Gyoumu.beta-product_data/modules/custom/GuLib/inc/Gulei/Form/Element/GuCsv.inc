<?php
namespace Gulei\Form\Element;

class GuCsv extends _Abstract
{
  public function getEntry()
  {
    $tpl = $this->getEntryTemplate();
    $tpl['#arch'] = $this->nodeValue;
    return $tpl;
  }
}