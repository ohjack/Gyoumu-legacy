<?php
namespace Ebapi\Fitment;

class Helper
{
  public function queryByEpid($ePID)
  {
    $sql = 'SELECT * FROM {ebapi_fitment} WHERE `ePID`="%s" LIMIT 1';
    $rs = db_query($sql, $ePID);
    return db_fetch_object($rs);
  }

  public function queryByItemID($accnt, $ItemID)
  {
    $listing = new Listing(new \Ebapi\Account\Helper($accnt), $ItemID);
    $fitment = array();

    foreach($listing->getCompList() as $each){
      $cri = array();
      foreach($each['NameValueList'] as $pair){
        $cri[$pair['Name']] = $pair['Value'];
      }

      $q = new QueryFitment($cri, null);
      foreach($q->getFitmentList() as $ePid => $name){
        $fitment[$ePid] = $name;
      }
    }
    asort($fitment);
    return $fitment;
  }
}