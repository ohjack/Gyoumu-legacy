<?php
namespace Oerp\Delivery\Page\Order\Callee;

class Save extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $vals = $form_state['values']['details'];
    $this->FormPage->save($vals);
    drupal_goto($_GET['q'], array('id' => $_GET['id']));
  }
}