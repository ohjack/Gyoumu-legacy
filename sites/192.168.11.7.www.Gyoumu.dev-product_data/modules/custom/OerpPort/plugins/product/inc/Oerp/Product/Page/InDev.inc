<?php
namespace Oerp\Product\Page;

class InDev extends \Oerp\Page\TreeView
{
  private function getArch()
  {
    return file_get_contents(__DIR__ . '/InDev.xml');
  }

  protected function hook_form_internal()
  {
    $this->args = array(0, array('arch' => $this->getArch()));
    return parent::hook_form_internal();
  }
}