<?php
namespace Oerp\Page\TreeView\Callee;

class Save extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $recs = \Oerp\Util\Helper::jsondecode(
      $form_state['clicked_button']['#post']['view']['json']);

    $recs = \Oerp\Util\Helper::renderNestedInput($recs);
    $param = $form['view']['#obj']->renew()->getParam();

    foreach ($recs as $rec) {
      $q = new \Oerp\Query\Basic($param['model'], null, null, array($rec[1]));

      switch ($rec[0]) {
        case 0: //create
          $q->create($rec[2]);
          break;

        case 1: //edit
          $q->write($rec[2]);
          break;

        case 2: //delete
          $q->unlink();
          break;
      }
    }
    $this->FormPage->goBack(false);
  }
}