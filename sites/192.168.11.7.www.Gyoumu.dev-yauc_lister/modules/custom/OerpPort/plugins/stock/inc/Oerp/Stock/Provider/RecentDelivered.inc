<?php
namespace Oerp\Stock\Provider;

class RecentDelivered implements \Gulei\Provider\_Interface
{
  private $ids;
  private $cri;
  private $period;
  private $offset;
  const date_pat = 'Y-m-d H:i:s';

  public function __construct($ids, $cri = array(), $offset = 1, $period = 7)
  {
    $this->ids = $ids;
    $this->cri = $cri;
    $this->offset = $offset;
    $this->period = $period;
  }

  private function getDate($offset)
  {
    $day_back = $offset * $this->period;
    return date(self::date_pat, mktime(0, 0, 0, date('m'), date('d') - $day_back));
  }

  private function getQuery()
  {
    $from_date = $this->getDate($this->offset);
    $to_date = $this->getDate($this->offset - 1);

    $qparam = array(
      'from_date' => $from_date,
      'to_date' => $to_date,
      'location' => 8,
    );

    $flds = array(
      'qty_available',
      'virtual_available',
      'uom_id',
    );

    return new \Oerp\Query\Basic(
      'product.product', $this->cri, $flds, $this->ids, $qparam);
  }

  public function getData()
  {
    $q = $this->getQuery();
    return $q->raw()->promote('id');
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