<?php
namespace Oerp\Gallery\Page\MappingPublish\Callee;

class Next_page extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $form_state['storage']['page']++;
    return true;
  }
}