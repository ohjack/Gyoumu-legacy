<?php
namespace Oerp\Gallery\Page\MappingPublish\Callee;

class Search extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $form_state['rebuild'] = true;
  }
}