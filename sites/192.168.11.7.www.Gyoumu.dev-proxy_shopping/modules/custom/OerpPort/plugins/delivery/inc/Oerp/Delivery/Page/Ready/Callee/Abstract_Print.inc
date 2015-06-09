<?php
namespace Oerp\Delivery\Page\Ready\Callee;

abstract class Abstract_Print extends \Gulei\Page\Form\Callee\_Abstract {
  abstract protected function getPrintCall();

  public function execute($form, &$form_state) {
    $sels = $this->FormPage->getSels();
    $recs = array();

    foreach ($sels as $id) {
      $recs[] = array('id' => $id);
    }

    $Call = $this->getPrintCall();
    $Call->printOrders(new \Gulei\Provider\Lambda($recs));
    drupal_goto('oerp/delivery/controlpanel');
  }
}