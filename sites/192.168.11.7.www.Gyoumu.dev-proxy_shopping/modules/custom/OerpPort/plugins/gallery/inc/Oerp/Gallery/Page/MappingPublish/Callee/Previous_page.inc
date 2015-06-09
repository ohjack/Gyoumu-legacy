<?php
namespace Oerp\Gallery\Page\MappingPublish\Callee;

class Previous_page extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $form_state['storage']['page']--;
    return true;
  }
}