<?php
namespace Oerp\Product\Page;

class PurchasePricelist extends \Oerp\Page\TreeView
{
  protected function hook_form_internal()
  {
    $arch = file_get_contents(__DIR__ . '/PurchasePricelist.xml');
    $this->args = array(0, array('arch' => $arch));
    return parent::hook_form_internal();
  }
}