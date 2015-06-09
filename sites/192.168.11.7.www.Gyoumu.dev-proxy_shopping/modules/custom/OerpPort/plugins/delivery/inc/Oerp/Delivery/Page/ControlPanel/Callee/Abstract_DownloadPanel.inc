<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee;

abstract class Abstract_DownloadPanel extends \Gulei\Page\Form\Component\_Abstract
{
  abstract protected function getCallees();

  protected function getEleTpl()
  {
    $ele = array(
      '#name' => $this->getCaption(),
      '#themer' => "Gulei\\Page\\Themer\\Pane",
    );

    foreach($this->getCallees() as $call){
      $ele['download'][$call->getEleName()] = $call->getEle();
    }
    return $ele;
  }
}