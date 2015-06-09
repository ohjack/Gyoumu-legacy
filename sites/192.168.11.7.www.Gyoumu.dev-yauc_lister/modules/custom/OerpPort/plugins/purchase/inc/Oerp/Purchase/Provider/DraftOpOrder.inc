<?php
namespace Oerp\Purchase\Provider;

class DraftOpOrder implements \Gulei\Provider\_Interface
{
  private function getQuery()
  {
    return new \Oerp\Query\Basic(
      'purchase.order',
      array(
        array('origin', 'like', 'OP/'),
        array('state', '=', 'draft')
      )
    );
  }

  public function getData()
  {
    return $this->getQuery()->raw();
  }

  public function count()
  {
    return $this->getQuery()->count();
  }

  public function getReference()
  {
    return array();
  }
}