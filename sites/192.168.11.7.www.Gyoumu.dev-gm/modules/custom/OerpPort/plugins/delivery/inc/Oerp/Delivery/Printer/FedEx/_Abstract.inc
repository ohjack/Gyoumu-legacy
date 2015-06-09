<?php
namespace Oerp\Delivery\Printer\FedEx;

abstract class _Abstract
{
  protected $prov;
  const LISTENERPATH = '/home/jeremy/FedexListener';

  public function __construct($prov)
  {
    $this->prov = $prov;
  }

  protected function getFilename($packid, $ext)
  {
    $classname = str_replace("\\", '_', __CLASS__);
    return $classname . '-' . $packid . '.' . $ext;
  }
}