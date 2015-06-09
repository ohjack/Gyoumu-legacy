<?php
namespace Yauc\Lister\Driver;

class Basic extends \AbstractYaucListingDriver
{
  private $rec;

  protected function getVarExp($name)
  {
    return sprintf('javascript{storedVars["%s"]}', $name);
  }

  protected function runScript($code)
  {
    $this->doCommand('runScript', array(sprintf('javascript{%s}', $code)));
  }

  protected function typeLongText($loc, $text)
  {
    $this->runScript('storedVars["longtext"] = "";');
    preg_match_all('/[\s\S]{1,1000}>/u', $text, $m);

    foreach($m[0] as $chunk){
      $cmd = "storedVars['longtext'] = storedVars['longtext'].concat('" . $chunk . "');";
      $this->runScript($cmd);
    }
    $this->doCommand('type', array($loc, $this->getVarExp('longtext')));;
  }

  private function selectLastOption($xpSel)
  {
    $valOpt = $this->getString('getValue', array("$xpSel/option[last()]"));
    $this->doCommand('select', array($xpSel, 'value=' . $valOpt));
  }

  private function enterTitle()
  {
    $title = trim($this->rec['title']);
    $this->doCommand('type', array('id=auc_title', $title));
  }

  private function enterDesc()
  {
    $this->doCommand('click', array("link=HTMLタグ入力"));
    $this->typeLongText("name=Description_plain_work", $this->rec['HTML']);
  }

  private function enterFormat()
  {
    $this->doCommand('click', array("id=" . $this->rec['format']));
  }

  private function enterPrice()
  {
    $this->doCommand('type', array("id=auc_StartPrice", $this->rec['start_price']));
    $this->doCommand('type', array("id=auc_BidOrBuyPrice", $this->rec['buynow_price']));
  }

  private function enterClosetime()
  {
    $this->selectLastOption("xpath=//select[@id='ClosingDate']");
    $this->selectLastOption("xpath=//select[@id='ClosingTime']");
  }

  private function enterShipFrom()
  {
    if($loc = $this->rec['location']){
      $this->doCommand('select', array("id=auc_loc_cd", "value=" . $loc));
    }
    else{
      $this->doCommand('select', array("id=auc_loc_cd", "label=海外"));
    }

    if($city = $this->rec['city']){
      $this->doCommand('type', array('name=city', $city));
    }
    else{
      $this->doCommand('type', array('name=city', ''));
    }
  }

  private function enterShippingFeePayer()
  {
    $this->doCommand('click', array($this->rec['shipping_term']));
  }

  private function enterPaymentStrategy()
  {
    $this->doCommand('click', array('id=auc_shiptime_payment'));
  }

  private function enterBankName()
  {
    $this->doCommand('type', array("id=auc_bkname1", "ジャパンネットバンク"));
    $this->doCommand('type', array("id=auc_bkname2", ""));
    $this->doCommand('type', array("id=auc_bkname3", ""));
  }

  private function enterItemStatus()
  {
    $this->doCommand('click', array("id=istatus_new"));
  }

  private function enterReturnPolicy()
  {
    $this->doCommand('click', array("id=retpolicy_no"));
    $this->doCommand('type', array("id=auc_retpolicy_comment", "ただし、初期不良の場合は写真判断後の返品を行います"));
  }

  private function enterShippingParam()
  {
    if($this->rec['intlOK']){
      $this->doCommand('check', array("id=auc_intlOK"));
    }
    else{
      $this->doCommand('uncheck', array("id=auc_intlOK"));
    }

    $locShipname = "id=auc_shipname1";
    $this->doCommand('type', array($locShipname, $this->rec['carrier']));
    $this->doCommand('keyUp', array($locShipname, '1'));
    $this->doCommand('click', array("id=auc_uniform_fee_set1_a"));
    $this->doCommand('type', array("id=auc_uniform_fee", $this->rec['shipping_fee']));
    $this->doCommand('keyUp', array("id=auc_uniform_fee", 'a'));
    $this->doCommand('click', array("id=uniform_fee_okbtn"));

    if($this->isTextPresent('（税込み）［編集｜削除］')){
      return true;
    }
    else{
      return false;
    }
  }

  private function enterResubmit()
  {
    $this->doCommand('select', array('name=numResubmit', 'value=' . $this->rec['relist']));
  }

  private function enterPicture()
  {
    $this->doCommand('clickAndWait', array('xpath=//input[@value="画像登録画面へ"]'));

    $locFile = "name=ImageFile1";
    $this->doCommand('type', array($locFile, $this->rec['pic_path']));
    $this->doCommand('clickAndWait', array("id=cnfm_btn"));
    $this->doCommand('clickAndWait', array("id=back_btn"));
    $this->doCommand('clickAndWait', array("xpath=//input[@value='商品情報入力に戻る']"));
  }

  private function confirmForm()
  {
    $this->doCommand('clickAndWait', array("id=auc_submit2"));
  }

  private function checkConfirm(){
    if($this->isElementPresent('id=auc_preview_submit')){
      return true;
    }

    $msg = $this->getString('getText', array('xpath=//div[@class="decErrorBox"]'));
    return array('msg' => $msg);
  }

  private function isInForm()
  {
    if($this->isTextPresent('以下のカテゴリに出品します。')){
      return true;
    }
    return false;
  }

  public function gotoCategoryUseCache($anchor)
  {
    $ctrl = new \Yauc\Lister\Category\Helper();
    $link = $ctrl->getLink($anchor);

    if($link){
      $this->open($link);

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
    $this->open('http://auctions.yahoo.co.jp/jp/0-submit.html');

    foreach($anchor as $ptr){
      if(strtoupper($ptr['menu']) == 'ON' || strtoupper($ptr['menu']) == 'OFF'){
        break;
      }

      $xp = sprintf('xpath=(//a[text()="%s"])[%s]', $ptr['menu'], $ptr['POS']);

      if($this->isTextPresent($ptr['menu'])){
        $this->doCommand('clickAndWait', array($xp));
      }
      else{
        return false;
      }
    }

    return $this->isInForm() ? $this->getLocation() : false;
  }

  function checkCategory($anchor)
  {
    $ctrl = new \Yauc\Lister\Category\Helper();
    $link = $this->gotoCategory($anchor);

    if($link){
      $src = $this->getSource('id=modCfmBox');
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
      $this->doCommand('type', array('name=featuredAmount', $featuredAmount));
    }
    else{
      $this->doCommand('type', array('name=featuredAmount', ''));
    }
  }

  private function confirmSubmit()
  {
    $this->doCommand('clickAndWait', array('id=auc_preview_submit'));
  }

  public function submit($rec, $break = false)
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
}