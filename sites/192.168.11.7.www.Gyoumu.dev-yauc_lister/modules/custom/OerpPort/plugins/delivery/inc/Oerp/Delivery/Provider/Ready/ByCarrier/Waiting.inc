<?php
namespace Oerp\Delivery\Provider\Ready\ByCarrier;

class Waiting extends \Oerp\Delivery\Provider\Ready\ByCarrier
{
  public function __construct($carriers, $fld = null, $ex_cri = array())
  {
    $cri = array_merge(array(array('x_tnum', '=', '_PRINT_')), $ex_cri);
    parent::__construct($carriers, $fld, $cri);
  }
}
