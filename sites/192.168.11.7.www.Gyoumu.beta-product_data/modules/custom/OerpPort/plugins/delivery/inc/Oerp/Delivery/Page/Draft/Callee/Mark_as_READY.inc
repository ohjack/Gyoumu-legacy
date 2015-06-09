<?php
namespace Oerp\Delivery\Page\Draft\Callee;

class Mark_as_READY extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $ids = $this->FormPage->getSelection();
    $q = new \Oerp\Query\Basic('stock.picking');
    $q->execute('draft_force_assign', $ids);
    $q->execute('force_assign', $ids);
  }
}