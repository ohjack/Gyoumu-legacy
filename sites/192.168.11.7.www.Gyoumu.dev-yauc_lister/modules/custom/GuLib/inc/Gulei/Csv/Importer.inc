<?php
namespace Gulei\Csv;

class Importer extends \Gulei\Page\Form\_Abstract
{
  protected function getClassname() {
    return __CLASS__;
  }

  protected function getImporter() {
    $imp_name = $this->form_state['storage']['importer_name'];

    if (!$imp_name) {
      throw new \Exception();
    }
    $imp_name = str_replace(':', "\\", $imp_name);
    return new $imp_name();
  }

  protected function hook_form_internal() {
    $imp = $this->getImporter();
    $t = new \Gulei\Csv\Buffer($imp->getArch(), $this->form_state);

    $form = array();
    $form['ex_ele'] = $imp->getEles($this->form_state);
    $form['#importer'] = $imp;
    $form = array_merge($form, $t->getFormEntry($this->form_state));
    return $form;
  }

  protected function getToolbarBottom() {
    return array();
  }

  public function getParam() {
    return array();
  }

  public function hook_form_validate($form, &$form_state) {
    $buf = $form['csv_file']['#obj']->renew();
    $buf->validateForm($form_state);
  }

  protected function proc_submit($form, &$form_state) {
    $imp = $this->getImporter();
    return $imp->submit($form, $form_state);
  }

  public function hook_theme($form) {
    $imp = $form['#importer'];
    return $imp->theme($form);
  }
}