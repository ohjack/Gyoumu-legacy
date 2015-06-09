<?php
namespace Oerp\Purchase\Page\Orders;

class Draft extends \Oerp\Purchase\Page\Orders
{
  protected function getArch()
  {
    return file_get_contents(__DIR__ . '/Draft.xml');
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