<?php

function GuFunc_getSku($recs, $ind, $rec, $name, $args){
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

function GuFunc_getTwTime($recs, $ind, $rec, $name, $args){
  if(!$rec[$name]){
    return 'Loading...';
  }

  $Tz = new DateTimeZone('Asia/Taipei');

  $Time = new DateTime($rec[$name]);
  $Time->setTimezone($Tz);
  return $Time->format('Y-m-d H:i:s');
}

function GuFunc_getRemainDays($recs, $ind, $rec, $name, $args){
  $Tz = new DateTimeZone('Asia/Taipei');
  $Now = new DateTime('now', $Tz);
  
  $EndTime = new DateTime($rec['time_end']);
  $EndTime->setTimezone($Tz);
   
  $remain_days =
    ((int)$EndTime->format('U') - (int)$Now->format('U')) / 86400;
    
  return $remain_days < 1 ? '< 1' : (int)$remain_days;
}

function GuFunc_getShippingMethod($recs, $ind, $rec, $name, $args){
  $src = unserialize($rec['src']);

  if($method = $src['Shipping']['Method']['Name']){
    return $method;
  }
  else{
    return $src['Shipping']['Method'][0]['Name'];
  }
}

function GuFunc_isCarrierFedex($recs, $ind, $rec, $name, $args){
  return preg_match('/^Fedex/', $rec[$name]);
}

function GuFunc_existValue($recs, $ind, $rec, $name, $args){
  $cri = json_decode($args[1]);
  
  $cri[] = array(
    $args[2], '=', $rec[$name]
  );
  
  $q = new OerpQuery($args[0], $cri, array('id'));
  $resp = (array)$q->raw();
  
  return !empty($resp);
}

function GuFunc_getFedexTrack($recs, $ind, $rec, $name, $args){
  if(GuFunc_isCarrierFedex($recs, $ind, $rec, 'carrier_id', $args)
     && $rec[$name] == '_PRINT_'){

    $resp = _fedprint_getRec($rec["name"], "tid");

    if($resp->tnum){
      OerpQuery::_execute(
        array(
             'stock.picking',
             'write',
             (int)$rec['id'],
             array('x_tnum' => $resp->tnum)
        )
      );
      return $resp->tnum;
    }
    else {
      return '<span class="gu_value">Waiting...</span>';
    }
  }
  else{
    return false;
  }
}

function GuFunc_getUpsTrack($recs, $ind, $rec, $name, $args){
  if($rec['carrier_id'] == 'UPS' && $rec[$name] == '_PRINT_'){
    $UpsCtrlr = new OerpUpsPrintController($rec['name']);

    if(!$UpsCtrlr->isSent()){
      return '<span class="gu_value">Waiting...</span>';
    }

    if($UpsCtrlr->isPrint()) {
      $trackNums = $UpsCtrlr->getTrackNum();

      if ($trackNums) {
        $t_num = implode("\n", $trackNums);

        OerpQuery::_execute(
          array(
               'stock.picking',
               'write',
               (int)$rec['id'],
               array('x_tnum' => $t_num)
          )
        );
        return '<span class="gu_value">' . nl2br($t_num) . '</span>';
      }
    }
    else {
      $msg = $UpsCtrlr->getLogMessage();

      if($msg == '_PRINT_'){
        return '<span class="notice">WAITING...</span>';
      }
      else{
        return sprintf(
          '<span class="invalid">ERROR</span><span class="show-tooltip-text">%s</span>',
          str_replace("\n", '<br/><br/>', $msg)
        );
      }
    }
  }
  return false;
}

function GuFunc_SearchOpMap($str){
  return str_replace(
    array('LT', 'LE', 'GT', 'GE', 'EQ'),
    array('<', '<=', '>', '>=', '='),
    $str
  );
}

function GuFunc_isEmptyString($recs, $ind, $rec, $name, $args){
  if(is_null($rec[$name]) || $rec[$name] === false){
    return true;
  }
  else if(strlen(trim($rec[$name])) == 0){
    return true;
  }
  return false;
}

function GuFunc_getFedexPrintDesc($recs, $ind, $rec, $name, $args){
  static $mapping;

  if(!$mapping){
    $csvDesc = new GuCsv(array(
      'filepath' => drupal_get_path('module', 'OerpDelivery') . '/FedexPrintDesc.csv',
    ));

    $mapping = array();
    foreach($csvDesc->connect() as $each){
      $pat = '/^' . trim($each['Category']) . '-.*/';
      $mapping[$pat] = $each['Description'];
    }
  }
  
  $skus = explode("\n", $rec["move_lines.product_id.default_code"]);
  $qtys = explode("\n", $rec["move_lines.product_qty"]);
  
  $pairs = array();
  foreach($skus as $ind => $sku){
    $desc = preg_replace(array_keys(($mapping)), $mapping, $sku);
    
    if($desc == $sku){
      $desc = 'Car Spoiler';
    }
    
    if(isset($pairs[$desc])){
      $pairs[$desc] += $qtys[$ind];
    }
    else{
      $pairs[$desc] = $qtys[$ind];
    }
  }
  
  foreach($pairs as $desc => &$pair){
    $pair = $desc . '(' . $pair . ')';
  }
  
  return implode(',', $pairs);
}

function GuFunc_beforeCloseTime($recs, $ind, $rec, $name, $args){
  $close = date('Y-m-d 00:00:00');
  return strcmp($close, $rec[$name]) > 0;
}

function GuFunc_isValidSku($recs, $ind, $rec, $name, $args){
  return !!preg_match('/^[A-Z]{2}-\d{5}$/', $rec[$name]);
}

?>