<?php
namespace Oerp\Page\Abstract_PeriodQuery\Callee;

class Search extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname() {
    return __CLASS__;
  }

  public function execute($form, &$form_state) {
    return TRUE;
  }
}