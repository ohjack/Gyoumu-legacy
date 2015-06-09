<?php
namespace Oerp\Delivery\Provider\Ready\FedEx;

class Error extends \Oerp\Delivery\Provider\Ready\FedEx
{
  public function __construct($ex_cri = array(), $fld = null)
  {
    $cri = array_merge(array(array('x_tnum', '=', '_ERROR_')), $ex_cri);
    parent::__construct($cri, $fld);
  }
}
