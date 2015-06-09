<?php
namespace Oerp\Delivery\Page\Callee;

abstract class Abstract_mark extends \Gulei\Page\Form\Callee\_Abstract
{
  abstract protected function mark();

  protected function getSels()
  {
    return $this->FormPage->getSels();
  }

  protected function getSelQuery()
  {
    return new \Oerp\Query\Basic('stock.picking', null, null, $this->getSels());
  }

  public function execute($form, &$form_state)
  {
    $this->mark();
    $qparam = $_GET;
    unset($qparam['q']);

    $form_state['redirect'] = array(
      $_GET['q'],
      http_build_query($qparam, '', '&')
    );

    drupal_set_message(
      sprintf('%s : %d record(s) .', $this->getCaption(), count($this->getSels())));
  }

  protected function getEleOverrides()
  {
    return array(
      '#attributes' => array('class' => 'needConfirm')
    );
  }
}