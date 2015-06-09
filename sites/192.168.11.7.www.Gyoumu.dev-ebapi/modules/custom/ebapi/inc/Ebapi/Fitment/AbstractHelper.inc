<?php
namespace Ebapi\Fitment;

abstract class AbstractHelper
{
  abstract public function queryById($id);
  abstract protected function getQuery($cri);

  public function queryByItemID($accnt, $ItemID)
  {
    $listing = new \Ebapi\Query\Listing(new \Ebapi\Account\Helper($accnt), $ItemID);
    $fitment = array();

    foreach($listing->getCompList() as $each){
      $cri = array();
      foreach($each['NameValueList'] as $pair){
        $cri[$pair['Name']] = $pair['Value'];
      }

      $q = $this->getQuery($cri);
      foreach($q->getFitmentList() as $ePid => $name){
        $fitment[$ePid] = $name;
      }
    }
    asort($fitment);
    return $fitment;
  }
}