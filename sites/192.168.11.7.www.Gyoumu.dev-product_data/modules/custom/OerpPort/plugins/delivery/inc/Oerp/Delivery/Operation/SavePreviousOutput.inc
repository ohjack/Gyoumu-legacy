<?php
namespace Oerp\Delivery\Operation;

class SavePreviousOutput extends \Oerp\Delivery\Operation\Abstract_InitDate {
  protected $days = 32;

  public function setDays($days) {
    $this->days = $days;
  }

  public function run() {
    for ($i = 1; $i <= $this->days; $i++) {
      $op = new \Oerp\Delivery\Operation\SaveDailyOutput(
        $this->year, $this->month, $this->day - $i, $this->override
      );
      $op->run();
    }
  }
}