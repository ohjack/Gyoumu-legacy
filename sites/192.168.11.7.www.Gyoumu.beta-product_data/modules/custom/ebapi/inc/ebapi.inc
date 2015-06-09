<?php
namespace Ebapi;



module_load_include('inc', 'GuLib', '/include/GuTable');
module_load_include('inc', 'GuLib', '/include/page');


abstract class AbstractApiCall
{
  protected $Account;

  public function __construct($Account)
  {
    $this->Account = $Account;
  }
}

class Listing extends AbstractApiCall
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


class PageMySelling extends AbstractPageMySelling
{
  protected function getArch()
  {
    return <<<XML
  <tree>
    <field name="ItemID"/>
    <field name="Site"/>
    <field name="SKU"/>
    <field name="Title"/>
    <field name="ListingType"/>
    <field name="BuyItNowPrice"/>
    <field name="CurrentPrice" string="Cur.Price"/>
    <field name="MinimumToBid" string="Min.Bid"/>
    <field name="ShippingServiceCost" string="S.Fee"/>
    <field name="QuantityAvailable" string="Qty"/>
    <field name="Quantity" string="Qty"/>
    <field name="StartTime"/>
  </tree>
XML;
  }

  protected function getAhahPath()
  {
    return 'ebapi/myebay/selling/js';
  }
}



class PageItem extends \FormPageController
{
  private $seller;
  private $ItemID;
  private $Item;
  private $vals;

  private function getItemObj()
  {
    if(is_null($this->Item)){
      $Acc = new \Ebapi\Account\Helper($this->seller);
      $this->Item = $Acc->getItem($this->ItemID);
    }
    return $this->Item;
  }

  private function getVals($anchors = null)
  {
    if(is_null($this->vals)){
      $this->vals = $this->getItemObj()->toArray();
    }

    if(is_null($anchors)){
      return $this->vals;
    }

    $anchors = explode('/', $anchors);
    $vals = $this->vals;
    $val = $vals[\array_shift($anchors)];

    foreach($anchors as $anchor){
      $val = $val[$anchor];
    }
    return $val;
  }

  private function getSendEles()
  {
    $arch = <<<XML
<fields>
  <field name="Title" type="textfield"/>
  <field name="SKU" type="textfield"/>
  <field name="Quantity" type="textfield"/>
</fields>
XML;

    $f = new \GuForm($arch, $this->getVals());
    return $f->getFormEles();
  }

  public function hook_form(&$form_state, $seller, $ItemID)
  {
    $this->seller = $seller;
    $this->ItemID = $ItemID;

    $form = array();
    $form['#tree'] = true;

    $form['obj'] = array(
      '#type' => 'value',
      '#value' => $this->getItemObj(),
    );

    $form['send'] = $this->getSendEles();

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Revise',
    );

    return $form;
  }

  protected function proc_submit($form, &$form_state)
  {
    $send = $form_state['values']['send'];
    $obj = $form['obj']['#value'];

    foreach($send as $key => $val){
      $obj->$key = $val;
    }
    $obj->Revise();

    drupal_set_message('Item revised successfully.');
  }
}