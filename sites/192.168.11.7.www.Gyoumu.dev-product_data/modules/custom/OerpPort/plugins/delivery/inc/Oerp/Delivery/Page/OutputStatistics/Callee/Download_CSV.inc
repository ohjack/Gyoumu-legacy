<?php
namespace Oerp\Delivery\Page\OutputStatistics\Callee;

class Download_CSV extends \Gulei\Page\Form\Callee\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  public function execute($form, &$form_state) {
    $vals = $form_state['values'];
    $t = $this->FormPage->getThemer($vals)->getTable();
    $fname = $t->getCsvFile(NULL, $this->getCaption());
    $form_state['storage']['redirect'] = $fname;
  }
}