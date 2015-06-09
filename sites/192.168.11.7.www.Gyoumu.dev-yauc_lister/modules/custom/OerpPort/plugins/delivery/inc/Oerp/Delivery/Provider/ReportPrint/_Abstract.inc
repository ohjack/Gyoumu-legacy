<?php
namespace Oerp\Delivery\Provider\ReportPrint;

abstract class _Abstract implements \Gulei\Provider\_Interface {
  const MODEL = 'stock.picking';

  abstract protected function getClassname();
  abstract protected function getThemeSrc();

  protected function getCarrierIds() {
    $carrs = explode('|', $_GET['carrier']);
    $cri = array(
      array('name', 'in', $carrs),
    );

    $q = new \Oerp\Query('delivery.carrier', $cri, array('id'));
    return $q->raw()->collect('id');
  }

  protected function getCriteria() {
    $carr_ids = $this->getCarrierIds();
    $cri = array(
      array('state', '=', 'assigned'),
      array('carrier_id', 'in', $carr_ids),
    );

    if ($_GET['after']) {
      $close_time = \Oerp\Delivery\Helper::getCloseTime('after');
      $cri[] = array('date', '>=', $close_time);
    }
    else {
      $close_time = \Oerp\Delivery\Helper::getCloseTime('before');
      $cri[] = array('date', '<=', $close_time);
    }
    return $cri;
  }

  protected function getTable() {
    $classname = $this->getClassname();
    $cname_toks = explode("\\", $classname);

    $arch = file_get_contents(__DIR__ . '/' . array_pop($cname_toks) . '.xml');
    $param = array(
      'oerp:criteria' => $this->getCriteria(),
      'oerp:op-type' => FALSE,
      'no-blank-tail' => TRUE,
    );

    return new \Oerp\Table\Arch($arch, $param);
  }

  public function getData() {
    return $this->getTable()->getRecordsAltered();
  }

  public function getReference() {
    return array();
  }

  public function count() {
    $q = new \Oerp\Query(self::MODEL, $this->getCriteria());
    return $q->execute('search_count', array());
  }
}