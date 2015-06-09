<?php
namespace Oerp\Delivery\Provider\Ready;

abstract class ByCarrier extends \Gulei\Provider\_Abstract
{
  protected $carriers;
  protected $ex_cri;
  protected $flds;
  private $close;

  public function __construct($carriers, $flds = null, $ex_cri = array(), $close = 'before')
  {
    $this->carriers = $carriers;
    $this->ex_cri = $ex_cri;
    $this->flds = $flds;
    $this->close = $close;
  }

  public function setExCri($cri)
  {
    $this->ex_cri = $cri;
  }

  protected function getCarrierIDs()
  {
    $q = new \Oerp\Query\Basic(
      'delivery.carrier', array(array('name', 'in', $this->carriers)), array('id'));

    return $q->raw()->collect('id');
  }

  private function getTimeCri() {
    if ($this->close == 'before') {
      $ts_close = mktime(23, 59, 59, date('m'), date('d') - 1, date('y'));
      $close = date('Y-m-d H:i:s', $ts_close);
      return array('date', '<=', $close);
    }

    if ($this->close == 'after') {
      $ts_close = mktime(0, 0, 0, date('m'), date('d'), date('y'));
      $close = date('Y-m-d H:i:s', $ts_close);
      return array('date', '>=', $close);
    }
    return false;
  }

  protected function getCriteria()
  {
    $cri = array(
      array('carrier_id', 'in', $this->getCarrierIDs()),
      array('state', '=', 'assigned'),
    );

    if ($time_cri = $this->getTimeCri()) {
      $cri[] = $time_cri;
    }

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
    return $this->createQuery()->raw(false, true);
  }

  public function getReference()
  {
    return array();
  }

  public function count()
  {
    $q = $this->createQuery(array('id'));
    return $q->raw(false, true)->count();
  }
}