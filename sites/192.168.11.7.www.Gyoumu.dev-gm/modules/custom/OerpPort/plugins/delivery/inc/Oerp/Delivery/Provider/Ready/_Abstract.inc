<?php
namespace Oerp\Delivery\Provider\Ready;

abstract class _Abstract implements \Gulei\Provider\_Interface
{
  protected $ex_cri;
  protected $flds;

  abstract protected function getCarrierNames();

  public function __construct($ex_cri = array(), $flds = null)
  {
    $this->ex_cri = $ex_cri;
    $this->flds = $flds;
  }

  public function setExCri($cri)
  {
    $this->ex_cri = $cri;
  }

  protected function getCarrierIDs()
  {
    $names = $this->getCarrierNames();
    $q = new \Oerp\Query\Basic('delivery.carrier', array(array('name', 'in', $names)), array('id'));
    return $q->raw()->collect('id');
  }

  protected function getCriteria()
  {
    $cri = array(
      array('carrier_id', 'in', $this->getCarrierIDs()),
      array('state', '=', 'assigned')
    );

    return array_merge($cri, $this->ex_cri);
  }

  protected function getFields()
  {
    return $this->flds;
  }

  protected function createQuery()
  {
    return new \Oerp\Query\Basic('stock.picking', $this->getCriteria(), $this->getFields());
  }

  public function getData()
  {
    return $this->createQuery()->raw();
  }

  public function getReference()
  {
    return array();
  }

  public function count()
  {
    $q = $this->createQuery(array('id'));
    return $q->raw()->count();
  }
}