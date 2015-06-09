<?php
namespace Oerp\Purchase\Command;

class FindNearestPoPlanDate extends \Oerp\Purchase\Command\Abstract_MoveWithOrder {
  public function execute($refVar = null){
    $move = $this->getMove($this->getSku());

    if ($move) {
      $plan_date = explode(' ', $move['date_planned']);
      return $plan_date[0];
    }
    else {
      return 'N/A';
    }
  }
}