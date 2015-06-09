<?php
namespace Oerp\Delivery\Iterator;

abstract class Abstract_Date {
  protected $year;
  protected $month;
  protected $day;
  protected $type;
  protected $endtime;
  protected $started = FALSE;

  abstract protected function _next();
  abstract protected function getRangeUnit();
  abstract protected function getIntervalStart();
  abstract protected function getIntervalEnd();
  abstract public function getColumnName();

  public function __construct($year, $month, $day, $type) {
    $this->year = $year;
    $this->month = $month;
    $this->day = $day;
    $this->type = $type;
  }

  public function setEndTime($year, $month, $day) {
    $this->endtime = mktime(23, 59, 59, $month, $day, $year);
  }

  public function next() {
    if ($this->started) {
      $this->_next();

      if ($this->endtime) {
        if( $this->endtime < $this->getIntervalStart()) {
          return FALSE;
        }
      }
      elseif (!$this->checkInRange($this->getRangeUnit())) {
        return FALSE;
      }
    }
    else {
      $this->started = TRUE;
    }
    return TRUE;
  }

  public function getWhere() {
    $cri = array();
    $cri[] = $this->type . ' >= ' . $this->getIntervalStart();
    $cri[] = $this->type . ' <= ' . $this->getIntervalEnd();
    return $cri;
  }

  protected function checkInRange($type) {
    switch ($type) {
      case 'year':
        $pat = 'Y';
        break;

      case 'month':
        $pat = 'm';
        break;

      case 'day':
        $pat = 'd';
        break;
    }

    $chk = date($pat, $this->getIntervalStart());

    if($this->$type > $chk){
      return FALSE;
    }
    return TRUE;
  }
}