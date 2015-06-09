<?php
namespace Oerp\Delivery\Provider\Ready\FedEx;

class Printed extends \Oerp\Delivery\Provider\Ready\FedEx
{
  public function __construct($ex_cri = array(), $fld = null)
  {
    $cri = array_merge(
      array(array('x_tnum', 'not in', array('_PRINT_', '_ERROR_', ''))), $ex_cri);
    parent::__construct($cri, $fld);
  }
}