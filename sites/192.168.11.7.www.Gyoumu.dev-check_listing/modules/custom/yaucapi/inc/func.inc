<?php 

function yaucapiFunc_getSku($recs, $ind, $rec, $name, $args){
  if(!$sku = $rec[$name]){
    $Rec = new YaucRecord($rec['auc_id']);
    $sku = $Rec->updateSku();
  }

  if($sku && $sku != 'UNKNOWN'){
    return $sku;
  }
  else{
    return '<span style="font-size: small; color: #ccc;"><span class="gu_value">UNKNOWN</span></span>';
  }
}

function yaucapiFunc_getTwTime($recs, $ind, $rec, $name, $args){
  if(!$rec[$name]){
    return 'Loading...';
  }

  $Tz = new DateTimeZone('Asia/Taipei');

  $Time = new DateTime($rec[$name]);
  $Time->setTimezone($Tz);
  return $Time->format('Y-m-d H:i:s');
}

function yaucapiFunc_getRemainDays($recs, $ind, $rec, $name, $args){
  $Tz = new DateTimeZone('Asia/Taipei');
  $Now = new DateTime('now', $Tz);
  
  $EndTime = new DateTime($rec['time_end']);
  $EndTime->setTimezone($Tz);
   
  $remain_days =
    ((int)$EndTime->format('U') - (int)$Now->format('U')) / 86400;
    
  return $remain_days < 1 ? '< 1' : (int)$remain_days;
}

function yaucapiFunc_getShippingMethod($recs, $ind, $rec, $name, $args){
  $src = unserialize($rec['src']);

  if($method = $src['Shipping']['Method']['Name']){
    return $method;
  }
  else{
    return $src['Shipping']['Method'][0]['Name'];
  }
}

?>
