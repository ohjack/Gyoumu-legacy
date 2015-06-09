<?php
namespace Oerp\Delivery\Page\Draft\Callee;

class Delete extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $ids = $this->FormPage->getSelection();
    $q = new \Oerp\Query\Basic('stock.picking', null, null, $ids);
    $q->unlink();
  }
}