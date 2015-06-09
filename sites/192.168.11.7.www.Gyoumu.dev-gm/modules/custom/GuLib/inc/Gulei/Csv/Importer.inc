<?php
namespace Gulei\Csv;

class Importer extends \Gulei\Page\Form\Basic
{
  public function hook_form(&$form_state, $imp_name)
  {
    $imp = new $imp_name;
    $t = new \Gulei\Csv\Buffer($imp->getArch(), $form_state);

    $form = $t->getFormEntry($form_state);
    $form['#importer_classname'] = $imp_name;
    return $form;
  }

  public function hook_form_validate($form, &$form_state)
  {
    $buf = $form['csv_file']['#obj']->renew();
    $buf->validateForm($form_state);
  }

  protected function proc_submit($form, &$form_state)
  {
    $imp_name = $form['#importer_classname'];
    $imp = new $imp_name();
    return $imp->submit($form, $form_state);
  }
}