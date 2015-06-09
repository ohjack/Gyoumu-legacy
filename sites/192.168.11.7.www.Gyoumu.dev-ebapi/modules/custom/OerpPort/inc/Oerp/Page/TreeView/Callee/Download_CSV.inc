<?php
namespace Oerp\Page\TreeView\Callee;

class Download_CSV extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $obj = $form['view']['#obj']->renew();
    $fname = 'Treeview-' . str_replace('.', '_', $obj->getParam('oerp:model'));

    $param = array(
      'oerp:op-type' => FALSE,
      'no-blank-tail' => TRUE,
      'items_per_page' => NULL,
    );

    $param = ($over = $form_state['clicked_button']['#param'])
        ? array_merge($param, $over)
        : $param;

    $form_state['storage']['redirect'] = $obj->getCsvFile($param, $fname);
  }
}