<?php
namespace Oerp\Delivery\Page\OutputStatistics\Callee;

class Query extends \Gulei\Page\Form\Callee\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  public function validate($form, &$form_state) {
    $form_state['rebuild'] = TRUE;
  }

  public function execute($form, &$form_state) { }
}