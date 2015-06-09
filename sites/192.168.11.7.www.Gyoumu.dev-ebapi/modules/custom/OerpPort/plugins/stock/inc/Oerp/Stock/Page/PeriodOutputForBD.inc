<?php
namespace Oerp\Stock\Page;

class PeriodOutputForBD extends \Oerp\Page\Abstract_PeriodQuery {
  public function getClassname() {
    return __CLASS__;
  }

  protected function getTitle() {
    return "Period Output";
  }

  protected function getProvider() {
    $vals = $this->getStateValues();
    return new \Oerp\Stock\Provider\PeriodOutput($vals);
  }

  protected function getTreeArch() {
    return file_get_contents(__DIR__.'/PeriodOutputForBD/PeriodOutputForBD.xml');
  }

  protected function getSearchEle() {
    $vals = $this->getStateValues();

    $search = parent::getSearchEle();
    $search['supp'] = array(
      '#title' => 'Supplier',
      '#type' => 'textfield',
      '#size' => '',
      '#default_value' => $vals['supp'],
      '#weight' => 2.1
    );
    $search['prod_mgr'] = array(
      '#title' => 'Product Manager',
      '#type' => 'textfield',
      '#size' => '',
      '#default_value' => $vals['prod_mgr'],
      '#weight' => 2.2
    );
    return $search;
  }

  public function hook_validate($form, $form_state) {
    $vals = $this->getStateValues();

    foreach (array('date-from', 'date-to') as $fld_name) {
      if (!$vals[$fld_name]) {
        form_set_error($fld_name, "Date fields cannnot be empty.");
      }
    }

    if (!$vals['supp'] && !$vals['prod_mgr']) {
      $err_msg = 'Supplier and Product Manager cannot both be empty.';
      form_set_error('supp', $err_msg);
      form_set_error('prod_mgr', $err_msg);
    }
  }
}