<?php
namespace Oerp\Delivery\Iterator;

class Daily extends \Oerp\Delivery\Iterator\Abstract_Date{
  protected function getIntervalStart() {
    return mktime(0, 0, 0, $this->month, $this->day, $this->year);
  }

  protected function getIntervalEnd() {
    return mktime(23, 59, 59, $this->month, $this->day, $this->year);
  }

  protected function getRangeUnit() {
    return 'day';
  }

  public function _next() {
    $this->day++;
  }

  public function getColumnName() {
    $time = $this->getIntervalStart();
    return date('Y/n/j D', $time);
  }
}