<?php
namespace Yauc\Lister\Page\Queue\Callee;

abstract class Abstract_Mark_as extends \Gulei\Page\Form\Calee\_Abstract
{
  abstract protected function markAs($id);
  abstract protected function getMsgPattern();

  public function execute($form, &$form_state)
  {
    $sels = $this->FormPage->getSelection();
    foreach($sels as $id){
      $this->markAs($id);
    }
    drupal_set_message(sprintf($this->getMsgPattern(), count($sels)));
  }
}