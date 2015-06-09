<?php
namespace Yauc\Driver\Command;

class uploadAuction extends \Yauc\Driver\Command\_Abstract
{
  protected $rec;

  protected function initForm()
  {
    if(!$this->drv->isElementPresent('id=auc_insertion_ok')){
      return false;
    }

    $this->drv->doCommand('click', array('id=auc_insertion_ok'));
  }

  protected function isSubmitSuccess()
  {
    if($this->drv->isTextPresent('以下の商品の出品手続きが完了しました。ご利用ありがとうございました。'))
      return true;

    if($this->drv->isTextPresent('このオークションは、仮出品の扱いとなりました'))
      return true;

    return false;
  }

  private function enterTitle()
  {
    $title = trim($this->rec['title']);
    $this->drv->doCommand('type', array('id=auc_title', $title));
  }

  private function enterDesc()
  {
    $this->drv->doCommand('click', array("link=HTMLタグ入力"));
    $this->drv->typeLongText("name=Description_plain_work", $this->rec['HTML']);
  }

  private function enterFormat()
  {
    $this->drv->doCommand('click', array("id=" . $this->rec['format']));
  }

  private function enterPrice()
  {
    $buynow_price = $this->rec['buynow_price'];

    if($this->rec['format'] == 'salesmode_auction' && !$this->rec['start_price']){
      $start_price = $buynow_price;
      $buynow_price = null;
    }
    else if(isset($this->rec['start_price'])){
      $start_price = $this->rec['start_price'];
    }
    else{
      $start_price = null;
    }

    if(!is_null($start_price)){
      $this->drv->doCommand('type', array("id=auc_StartPrice", $start_price));
    }

    if(!is_null($buynow_price)){
      $this->drv->doCommand('type', array("id=auc_BidOrBuyPrice", $buynow_price));
    }
  }

  private function enterClosetime()
  {
    $this->drv->selectLastOption("xpath=//select[@id='ClosingDate']");
    $this->drv->selectLastOption("xpath=//select[@id='ClosingTime']");
  }

  private function enterShipFrom()
  {
    if($loc = $this->rec['location']){
      $this->drv->doCommand('select', array("id=auc_loc_cd", "value=" . $loc));
    }
    else{
      $this->drv->doCommand('select', array("id=auc_loc_cd", "label=海外"));
    }

    if($city = $this->rec['city']){
      $this->drv->doCommand('type', array('name=city', $city));
    }
    else{
      $this->drv->doCommand('type', array('name=city', ''));
    }
  }

  private function enterShippingFeePayer()
  {
    $this->drv->doCommand('click', array($this->rec['shipping_term']));
  }

  private function enterPaymentStrategy()
  {
    $this->drv->doCommand('click', array('id=auc_shiptime_payment'));
  }

  private function enterBankName()
  {
    $this->drv->doCommand('type', array("id=auc_bkname1", "ジャパンネットバンク"));
    $this->drv->doCommand('type', array("id=auc_bkname2", ""));
    $this->drv->doCommand('type', array("id=auc_bkname3", ""));
  }

  private function enterItemStatus()
  {
    $this->drv->doCommand('click', array("id=istatus_new"));
  }

  private function enterReturnPolicy()
  {
    $this->drv->doCommand('click', array("id=retpolicy_no"));
    $this->drv->doCommand('type', array("id=auc_retpolicy_comment", "ただし、初期不良の場合は写真判断後の返品を行います"));
  }

  private function enterShippingParam()
  {
    if($this->rec['intlOK']){
      $this->drv->doCommand('check', array("id=auc_intlOK"));
    }
    else{
      $this->drv->doCommand('uncheck', array("id=auc_intlOK"));
    }

    $locShipname = "id=auc_shipname1";
    $this->drv->doCommand('type', array($locShipname, $this->rec['carrier']));
    $this->drv->doCommand('keyUp', array($locShipname, '1'));
    $this->drv->doCommand('click', array("id=auc_uniform_fee_set1_a"));
    $this->drv->doCommand('type', array("id=auc_uniform_fee", $this->rec['shipping_fee']));
    $this->drv->doCommand('keyUp', array("id=auc_uniform_fee", 'a'));
    $this->drv->doCommand('click', array("id=uniform_fee_okbtn"));

    if($this->drv->isTextPresent('（税込み）［編集｜削除］')){
      return true;
    }
    else{
      return false;
    }
  }

  private function enterResubmit()
  {
    $this->drv->doCommand('select', array('name=numResubmit', 'value=' . $this->rec['relist']));
  }

  private function enterPicture()
  {
    $this->drv->doCommand('clickAndWait', array('xpath=//input[@value="画像登録画面へ"]'));

    $locFile = "name=ImageFile1";
    $this->drv->doCommand('type', array($locFile, $this->rec['pic_path']));
    $this->drv->doCommand('clickAndWait', array("id=cnfm_btn"));
    $this->drv->doCommand('clickAndWait', array("id=back_btn"));
    $this->drv->doCommand('clickAndWait', array("xpath=//input[@value='商品情報入力に戻る']"));
  }

  private function confirmForm()
  {
    $this->drv->doCommand('clickAndWait', array("id=auc_submit2"));
  }

  private function checkConfirm(){
    if($this->drv->isElementPresent('id=auc_preview_submit')){
      return true;
    }

    $msg = $this->drv->getString('getText', array('xpath=//div[@class="decErrorBox"]'));
    return array('msg' => $msg);
  }

  private function isInForm()
  {
    if($this->drv->isTextPresent('以下のカテゴリに出品します。')){
      return true;
    }
    return false;
  }

  public function gotoCategoryUseCache($anchor)
  {
    $ctrl = new \Yauc\Lister\Category\Helper();
    $link = $ctrl->getLink($anchor);

    if($link){
      $this->drv->open($link, true);

      if($this->isInForm()){
        return $link;
      }
      else{
        $ctrl->markAsBroken($anchor);
      }
    }
    return $this->checkCategory($anchor);
  }

  public function gotoCategory($anchor)
  {
    $this->drv->open('http://auctions.yahoo.co.jp/jp/0-submit.html');

    foreach($anchor as $ptr){
      if(strtoupper($ptr['menu']) == 'ON' || strtoupper($ptr['menu']) == 'OFF'){
        break;
      }

      $xp = sprintf('xpath=(//a[text()="%s"])[%s]', $ptr['menu'], $ptr['POS']);

      if($this->drv->isTextPresent($ptr['menu'])){
        $this->drv->doCommand('clickAndWait', array($xp));
      }
      else{
        return false;
      }
    }

    return $this->isInForm() ? $this->drv->getLocation() : false;
  }

  function checkCategory($anchor)
  {
    $ctrl = new \Yauc\Lister\Category\Helper();
    $link = $this->gotoCategory($anchor);

    if($link){
      $src = $this->drv->getSource('id=modCfmBox');
      preg_match('/以下のカテゴリに出品します。([\s\S]*)/', $src, $m);
      $name = trim($m[1]);

      $ctrl->updateRecord($name, $link, $anchor);
      return true;
    }

    $ctrl->markAsBroken($anchor);
    return false;
  }

  private function enterPromotion()
  {
    if($featuredAmount = $this->rec['featuredAmount']){
      $this->drv->doCommand('type', array('name=featuredAmount', $featuredAmount));
    }
    else{
      $this->drv->doCommand('type', array('name=featuredAmount', ''));
    }
  }

  private function confirmSubmit()
  {
    $this->drv->doCommand('clickAndWait', array('id=auc_preview_submit'));
  }

  protected function submit($rec, $break = false)
  {
    $this->rec = $rec;

    if(!$this->gotoCategoryUseCache($this->rec['category'])){
      return array('msg' => 'Failed to find category');
    }

    $this->initForm();

    $this->enterTitle();
    $this->enterDesc();
    $this->enterFormat();
    $this->enterPrice();
    $this->enterClosetime();
    $this->enterShipFrom();
    $this->enterShippingFeePayer();
    $this->enterPaymentStrategy();
    $this->enterBankName();
    $this->enterItemStatus();
    $this->enterReturnPolicy();

    $isShippingFeeEntered = false;
    for($c = 1; $c <= 3; $c++){
      if($isShippingFeeEntered = $this->enterShippingParam()){
        break;
      }
    }

    if(!$isShippingFeeEntered){
      return array('msg' => 'Failed to set shipping fee.');
    }

    $this->enterResubmit();
    $this->enterPromotion();
    $this->enterPicture();

    $this->confirmForm();

    if($break){
      return true;
    }

    $resp = $this->checkConfirm();
    if($resp !== true){
      return $resp;
    }

    $this->confirmSubmit();
    return $this->isSubmitSuccess() ? true : false;
  }

  protected function exe_internal()
  {
    list($rec, $isTrial) = $this->ref->vals('rec', 'isTrial');

    if($isTrial){
      $this->submit($rec, true);
      return array('success' => true, 'msg' => $rec['title']);
    }
    else{
      $resp = $this->submit($rec);
      $ctrl = new \Yauc\Lister\Query\Queue($rec['id']);

      if($resp === true){
        $ctrl->setDone();
        return array('success' => true, 'msg' => $rec['title']);
      }
      else{
        $ctrl->setFailed($resp['msg']);
        return array('success' => false, 'msg' => $rec['title']);
      }
    }
  }
}