<?php
namespace Oerp\Stock\Provider;

class PeriodStock extends \Gulei\Provider\_Abstract {
  const date_pat = 'Y-m-d H:i:s';

  public function __construct($cri = array(), $loc, $from_time, $to_time) {
    $this->cri = $cri;
    $this->loc = $loc;
    $this->from_time = $from_time;
    $this->to_time = $to_time;
  }

  private function getQuery() {
    $qparam = array(
      'from_date' => $this->from_time,
      'to_date' => $this->to_time,
      'location' => $this->loc,
    );

    $flds = array(
      'state',
      'default_code',
      'name',
      'qty_available',
      'virtual_available',
      'uom_id',
      'product_tmpl_id',
    );

    return new \Oerp\Query\Basic(
      'product.product', $this->cri, $flds, NULL, $qparam);
  }

  public function getData()
  {
    $q = $this->getQuery();
    return $q->raw(FALSE, TRUE)->promote('id');
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