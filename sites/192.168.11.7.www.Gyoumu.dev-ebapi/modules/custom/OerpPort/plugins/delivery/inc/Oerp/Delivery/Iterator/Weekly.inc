<?php
namespace Oerp\Delivery\Iterator;

class Weekly extends \Oerp\Delivery\Iterator\Abstract_Date{
  protected function getIntervalStart() {
    return mktime(0, 0, 0, $this->month, $this->day, $this->year);
  }

  protected function getIntervalEnd() {
    return mktime(23, 59, 59, $this->month, $this->day + 6, $this->year);
  }

  protected function getRangeUnit() {
    return 'day';
  }

  public function _next() {
    $this->day += 7;
  }

  public function getColumnName() {
    $date = date('Y/n/j', $this->getIntervalStart());
    return $date .'~';
  }
}