<?php
namespace Oerp\Delivery\Operation;

class SaveDailyOutput extends \Oerp\Delivery\Operation\Abstract_SaveOutput {
  protected function getTimeStart() {
    return mktime(0, 0, 0, $this->month, $this->day, $this->year);
  }

  protected function getTimeEnd() {
    return mktime(23, 59, 59, $this->month, $this->day, $this->year);
  }
}