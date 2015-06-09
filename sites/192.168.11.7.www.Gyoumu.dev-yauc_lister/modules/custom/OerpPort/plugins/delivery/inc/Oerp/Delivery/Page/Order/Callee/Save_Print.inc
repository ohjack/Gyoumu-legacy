<?php
namespace Oerp\Delivery\Page\Order\Callee;

class Save_Print extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getEleOverrides()
  {
    return array(
      '#value' => 'Save & Print',
    );
  }

  public function execute($form, &$form_state)
  {
    $vals = $form_state['values']['details'];
    $this->FormPage->save($vals);

    $RespCtrl = $this->FormPage->getResponseCtrl();
    $RecCtrl = $this->FormPage->getRecordCtrl();

    $Call = $RespCtrl->createCall();
    $Call->printOrders($RecCtrl);
    drupal_goto($_GET['q'], array('id' => $_GET['id']));
  }
}