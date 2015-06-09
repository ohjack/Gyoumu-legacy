<?php
namespace Oerp\Purchase\Provider;

class UnapprovedOrder implements \Gulei\Provider\_Interface {
  protected function getQuery() {
    return new \Oerp\Query\Basic(
      'purchase.order',
      array(
        array('state', 'in', array('draft', 'confirmed')),
      )
    );
  }

  public function getData() {
    return $this->getQuery()->raw();
  }

  public function count() {
    return $this->getQuery()->count();
  }

  public function getReference() {
    return array();
  }
}