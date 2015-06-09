<?php
namespace Oerp\Reference;

class ValuePair extends \Gulei\Reference\Basic
{
  public function addPair($model, $fldKey, $fldVal, $jsonCri = null)
  {
    $cri = is_array($cri_src = json_decode($jsonCri)) ? $cri_src : null;
    $q = new \Oerp\Query\Basic($model, $cri, array($fldKey, $fldVal));

    $pairs = array();
    foreach($q->raw() as $rec){
      $pairs[$rec[$fldKey]] = $rec[$fldVal];
    }

    $this->add(array($model => $pairs));
    return $this;
  }
}