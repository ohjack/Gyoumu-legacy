<?php
namespace Oerp\Product\Page;

class Packaging extends \Oerp\Page\TreeView
{
  protected function hook_form_internal()
  {
    $arch = file_get_contents(__DIR__ . '/Packaging.xml');
    $param = array('arch' => $arch);

    if(!is_null($_GET['pager']) && !$_GET['pager']){
      $param['pager'] = false;
    }

    $this->args = array(0, $param);
    return parent::hook_form_internal();
  }
}