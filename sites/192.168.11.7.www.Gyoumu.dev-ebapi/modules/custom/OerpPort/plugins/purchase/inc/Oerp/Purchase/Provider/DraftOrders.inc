<?php
namespace Oerp\Purchase\Provider;

class DraftOrders extends \Gulei\Provider\_Abstract {
  protected function getQuery() {
    return new \Oerp\Query\Basic(
      'purchase.order',
      array(
        array('state', 'in', array('draft')),
      )
    );
  }

  public function getData() {
    return $this->getQuery()->raw(FALSE, TRUE);
  }

  public function count() {
    return $this->getQuery()->count();
  }

  public function getReference() {
    return array();
  }
}