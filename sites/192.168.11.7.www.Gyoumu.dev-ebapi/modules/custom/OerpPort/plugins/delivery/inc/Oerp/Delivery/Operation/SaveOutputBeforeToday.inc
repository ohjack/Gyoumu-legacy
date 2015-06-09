<?php
namespace Oerp\Delivery\Operation;

class SaveOutputBeforeToday extends \Oerp\Delivery\Operation\SavePreviousOutput {
  public function __construct() {
    parent::__construct(date('Y'), date('m'), date('d'));
  }
}