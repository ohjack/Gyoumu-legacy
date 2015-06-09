<?php
namespace Oerp\Delivery\Operation;

abstract class Abstract_InitDate implements \Gulei\Operation\_Interface {
  const DATE_FORMAT = 'Y-m-d H:i:s';
  protected $year;
  protected $month;
  protected $day;
  protected $override;

  public function __construct($year, $month, $day, $override = FALSE) {
    $this->year = $year;
    $this->month = $month;
    $this->day = $day;
    $this->override = $override;
  }
}