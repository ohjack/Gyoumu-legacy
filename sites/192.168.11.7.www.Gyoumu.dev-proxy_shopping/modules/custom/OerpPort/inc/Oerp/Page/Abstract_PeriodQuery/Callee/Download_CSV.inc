<?php
namespace Oerp\Page\Abstract_PeriodQuery\Callee;

class Download_CSV extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname() {
    return __CLASS__;
  }

  public function execute($form, &$form_state) {
    $fname = explode("\\", $this->FormPage->getClassname());
    $fname = array_pop($fname);

    $param = array(
      'copy_method' => 'duplicate',
      'no-blank-tail' => TRUE,
    );

    $Tb = $this->FormPage->getTableObj();
    $form_state['storage']['redirect'] = $Tb->getCsvFile($param, $fname);
  }
}