<?php
namespace Oerp\Delivery\Provider\Ready\ByCarrier;

class Printed extends \Oerp\Delivery\Provider\Ready\ByCarrier
{
  public function __construct($carriers, $fld = null, $ex_cri = array())
  {
    $cri = array_merge(
      array(array('x_tnum', 'not in', array('_PRINT_', '_ERROR_', ''))), $ex_cri);
    parent::__construct($carriers, $fld, $cri, FALSE);
  }
}