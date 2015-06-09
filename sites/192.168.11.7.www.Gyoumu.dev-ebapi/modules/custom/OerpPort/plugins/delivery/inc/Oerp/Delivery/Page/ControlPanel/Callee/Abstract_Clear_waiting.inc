<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee;

abstract class Abstract_Clear_waiting extends \Gulei\Page\Form\Callee\_Abstract
{
  abstract protected function getCarriers();

  protected function getEleOverrides()
  {
    return array(
      '#value' => 'Clear',
    );
  }

  public function execute($form, &$form_state)
  {
    $h = new \Oerp\Delivery\Provider\Ready\Helper();
    $prov = $h->createProvider($this->getCarriers(), 'Waiting', array('id'));
    $ids = $prov->getData()->collect('id');

    $q = new \Oerp\Query\Basic('stock.picking', null, null, $ids);
    $q->write(array('x_tnum' => ''));
  }
}