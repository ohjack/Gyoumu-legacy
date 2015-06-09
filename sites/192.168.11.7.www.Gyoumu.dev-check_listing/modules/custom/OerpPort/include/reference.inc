<?php
namespace Oerp\Reference;

require_once __DIR__ . '/query.inc';

class ValuePair extends \Gulei\Reference
{
  public function addPair($model, $fldKey, $fldVal, $jsonCri = null)
  {
    $cri = is_array($cri_src = json_decode($jsonCri)) ? $cri_src : null;
    $q = new \OerpQuery($model, $cri, array($fldKey, $fldVal));

    $pairs = array();
    foreach($q->raw() as $rec){
      $pairs[$rec[$fldKey]] = $rec[$fldVal];
    }

    $this->add(array($model => $pairs));
    return $this;
  }
}