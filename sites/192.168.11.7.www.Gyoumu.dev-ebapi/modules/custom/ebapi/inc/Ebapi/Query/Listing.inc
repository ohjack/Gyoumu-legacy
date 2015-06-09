<?php
namespace Ebapi\Query;

class Listing extends _Abstract
{
  protected $ItemID;
  protected $item;

  public function __construct($Account, $ItemID)
  {
    parent::__construct($Account);
    $this->ItemID = $ItemID;
  }

  public function getDetails($param = array())
  {
    $this->item = \Services_Ebay::loadModel('Item', $this->ItemID, $this->Account->getSession());
    $this->item->Get($param);
    return $this->item->toArray();
  }

  private function renderFitment($comp)
  {
    $vlist = &$comp['NameValueList'];
    $newVlist = array();

    foreach($vlist as $each){
      if($name = $each['Name']){
        $newVlist[$name] = $each;
      }
    }
    $vlist = $newVlist;
    return $comp;
  }

  public function getCompList()
  {
    $resp = $this->getDetails(array('IncludeItemCompatibilityList' => '1'));
    $comp = $resp['ItemCompatibilityList']['Compatibility'];
    $list = array();

    if(isset($comp['NameValueList'])){
      return array($this->renderFitment($comp));
    }
    else if(is_array($comp)){
      foreach($comp as $eachComp){
        $list[] = $this->renderFitment($eachComp);
      }
    }
    return $list;
  }
}