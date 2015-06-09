<?php

function OerpFunc_isCarrierFedex($recs, $ind, $rec, $name, $args){
  return preg_match('/^Fedex/', $rec[$name]);
}

function OerpFunc_isCarrierUps($recs, $ind, $rec, $name, $args){
  return $rec[$name] == 'UPS';
}

function OerpFunc_existValue($recs, $ind, $rec, $name, $args){
  $cri = json_decode($args[1]);
  
  $cri[] = array(
    $args[2], '=', $rec[$name]
  );
  
  $q = new OerpQuery($args[0], $cri, array('id'));
  $resp = (array)$q->raw();
  
  return !empty($resp);
}

function OerpFunc_getFedexTrack($recs, $ind, $rec, $name, $args){
  if(OerpFunc_isCarrierFedex($recs, $ind, $rec, 'carrier_id', $args)
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

function OerpFunc_getUpsTrack($recs, $ind, $rec, $name, $args){
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
          '<div><span class="invalid">ERROR</span><span class="show-tooltip-text">%s</span></div>',
          str_replace("\n", '<br/><br/>', $msg)
        );
      }
    }
  }
  return false;
}

function OerpFunc_SearchOpMap($str){
  return str_replace(
    array('LT', 'LE', 'GT', 'GE', 'EQ'),
    array('<', '<=', '>', '>=', '='),
    $str
  );
}

function OerpFunc_isEmptyString($recs, $ind, $rec, $name, $args){
  if(is_null($rec[$name]) || $rec[$name] === false){
    return true;
  }
  else if(strlen(trim($rec[$name])) == 0){
    return true;
  }
  return false;
}

function OerpFunc_getFedexPackageType($recs, $ind, $rec, $name, $args)
{
  static $mapping;

  if(!$mapping){
    $csvPkg = new GuCsv(array(
      'filepath' => drupal_get_path('module', 'OerpDelivery') . '/FedexPackaging.csv',
    ));

    $ArryMapping = new GuArray($csvPkg->connect());
    $mapping = (array)$ArryMapping->promote('SKU');
  }

  $skus = explode("\n", $rec["move_lines.product_id.default_code"]);
  $qtys = explode("\n", $rec["move_lines.product_qty"]);

  if(count($skus) > 1 || $qtys[0] > 1 || !isset($mapping[$skus[0]])){
    if($name == 'func:pkg_type'){
      return '01';
    }
    else if($name == 'func:adm_pkg_type'){
      return 'PCS';
    }
  }
  else{
    $pkg_map = array(
      'Fedex Box' => array('03','BOX'),
      'Fedex Tube' => array('04', 'TBE'),
    );

    $type = $mapping[$skus[0]]['packaging'];

    if($name == 'func:pkg_type'){
      $pack = $pkg_map[$type][0];
    }
    else if($name == 'func:adm_pkg_type'){
      $pack = $pkg_map[$type][1];
    }

    if(!$pack){
      throw new Exception('getFedexPackageType: Unknown packaging type.');
    }
    return $pack;
  }
}

function OerpFunc_getFedexPrintDesc($recs, $ind, $rec, $name, $args){
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

function OerpFunc_beforeCloseTime($recs, $ind, $rec, $name, $args){
  $close = date('Y-m-d 00:00:00');
  return strcmp($close, $rec[$name]) > 0;
}


function OerpFunc_appendChkCode($recs, $ind, $rec, $name, $args){
  $val = $rec[$name];

  preg_match("/^PACK(\d*)/", $val, $m);

  if($m[1]){
    return $val . chr((int)$m[1] % 26 + 65);
  }
  return false;
}

function OerpFunc_isValidSku($recs, $ind, $rec, $name, $args){
  return !!preg_match('/^[A-Z]{2}-\d{5}$/', $rec[$name]);
}

?>