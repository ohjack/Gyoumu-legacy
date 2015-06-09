<?php
namespace Oerp\Delivery\Provider\Ready;

class UpsPrintable extends \Oerp\Delivery\Provider\Ready\UPS
{
  public function __construct()
  {
    $ex_cri = array(array('x_tnum', '=', ''));
    parent::__construct($ex_cri, array('id'));
  }
}