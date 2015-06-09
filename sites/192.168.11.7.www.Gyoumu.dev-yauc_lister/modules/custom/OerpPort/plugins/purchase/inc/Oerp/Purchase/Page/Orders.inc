<?php
namespace Oerp\Purchase\Page;

class Orders extends \Oerp\Page\TreeView
{
  protected function getArch()
  {
    return file_get_contents(__DIR__ . '/Orders.xml');
  }

  public function hook_form_internal()
  {
    $param = array(
      'arch' => $this->getArch()
    );

    $this->args = array(0, $param);
    return parent::hook_form_internal();
  }

  protected function getToolbarBottom()
  {
    $form = parent::getToolbarBottom();

    $callee_download = 'ns:Oerp:Page:TreeView:Callee:Download_CSV';

    if($form[$callee_download]){
      $form[$callee_download]['#param'] = array(
        'copy_method' => 'duplicate'
      );
    }
    return $form;
/*
    $basename = __CLASS__ . "\\Callee\\";
    $name = $basename . "Merge";

    $Call = new $name($this);
    $form[$Call->getEleName] = $Call->getEle();
    return $form;
*/
  }
}