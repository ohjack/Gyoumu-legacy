<?php
namespace Oerp\Delivery\Page\Overview\Callee;

class Clear_FedEx_waiting extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $prov = new \Oerp\Delivery\Provider\Ready\FedEx\Waiting(array(), array('id'));
    $ids = $prov->getData()->collect('id');
    $q = new \Oerp\Query\Basic('stock.picking', null, null, $ids);
    $q->write(array('x_tnum' => ''));
  }

  protected function getEleOverrides()
  {
    return array(
      '#value' => 'Clear',
    );
  }
}