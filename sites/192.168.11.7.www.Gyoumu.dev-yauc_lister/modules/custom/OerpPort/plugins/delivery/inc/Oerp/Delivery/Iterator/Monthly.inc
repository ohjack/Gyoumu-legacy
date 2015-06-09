<?php
namespace Oerp\Delivery\Iterator;

class Monthly extends \Oerp\Delivery\Iterator\Abstract_Date{
  protected function getIntervalStart() {
    return mktime(0, 0, 0, $this->month, $this->day, $this->year);
  }

  protected function getIntervalEnd() {
    return mktime(23, 59, 59, $this->month + 1, $this->day - 1, $this->year);
  }

  protected function getRangeUnit() {
    return 'month';
  }

  public function _next() {
    $this->month++;
  }

  public function getColumnName() {
    return date('M', $this->getIntervalStart());
  }
}