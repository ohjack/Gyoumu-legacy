<?php

module_load_include('inc', 'GuLib', 'include/GuText');

class YaucJpModel
{
  static function inc(){
    drupal_add_js(drupal_get_path('module', 'yaucjp') . '/yaucjp.js');
    drupal_add_css(drupal_get_path('module', 'yaucjp') . '/yaucjp.css');
  } 
  
  static function getContactFormEles($form_state, $sellerId){
    $arch = file_get_contents(
      drupal_get_path('module', 'yaucjp') . '/contactForm.xml');
      
    $ele_name = 'contact_form';
    $defaults = $form_state['values'][$ele_name];
    
    $f = new GuForm($arch, $defaults);
    $form = $f->getFormEntry($ele_name);
    
    $form['sellerId'] = array(
      '#type' => 'value',
      '#value' => $sellerId,
    );
    
    return $form;
  }
  
  static public function themeContactForm(&$form){
    $output = drupal_render($form);
    return '<div style="width: 600px;">' . $output . '</div>';
  }
  
  static public function getConfirmFormEles(&$form_state){
    $vals = $form_state['values']['contact_form'];

    $rows = array();
    $rows['yid'] = array('YAHOO ID', $vals['yid']);
    
    $aids = explode("\n", $vals['aid']);
    $rows['aid'] = array('Auction ID', implode('<br />', $aids));
    $rows['email'] = array('E-mail', $vals['email']);
    
    $rows['price'] = array('決済金額', sprintf('落札金額　%s<br/>送　　料　%s<br/>合　　計　%s',
      $vals['price_bid'], $vals['price_ship'], $vals['price_bid'] + $vals['price_ship']
    ));
        
    $rows['name'] = array('Name', sprintf('%s %s (%s %s)',
      $vals['name_kanji_last'], $vals['name_kanji_first'],
      $vals['name_kana_last'], $vals['name_kana_first']
    ));
        
    $rows['phone'] = array('Phone', $vals['phone']);
    $rows['addr'] = array('Address', sprintf('%s %s %s %s ',
      $vals['zip'], $vals['addr_todoufuken'], $vals['addr_sichouson'], $vals['addr_chouiki']));
      
    $addr2 = array($vals['addr_choume'], $vals['addr_banchi'], $vals['addr_gou']);
    $addr2 = array_filter($addr2);
    
    $rows['addr'][1] .= implode('-', $addr2);
    
    if($vals['addr_house']){
      $rows['addr'][1] .= ' ' . $vals['addr_house'];
    }
    
    if($vals['addr_floor']){
      $rows['addr'][1] .= ' ' . $vals['addr_floor'] . 'F';
    }

    if($vals['addr_room']){
      $rows['addr'][1] .= '-' . $vals['addr_room'];
    }
    
    $rows['msg'] = array('Message', $vals['msg']);
    
    $eles = array();
    
    $eles['overview'] = array(
      '#type' => 'markup',
      '#value' => theme_table(null, $rows),
    );
    
    $eles['submit'] = array(
      '#type' => 'submit',
      '#name' => 'submit',
      '#value' => '確認する',
    );
    
    $eles['back'] = array(
      '#type' => 'submit',
      '#name' => 'back',
      '#value' => '戻る'
    );
    
    $vals['sellerId'] = $form_state['values']['sellerId'];
    $form_state['storage']['vals'] = $vals;
    $form_state['storage']['step'] = 'submit';
    return $eles;
  }
  
  function getAddr2($vals){
    $addr2 = array($vals['addr_choume'], $vals['addr_banchi'], $vals['addr_gou']);
    $addr2 = array_filter($addr2);
    
    $output = '';
    $output .= implode('-', $addr2);
    
    if($vals['addr_house']){
      $output .= ' ' . $vals['addr_house'];
    }
    
    if($vals['addr_floor']){
      $output .= ' ' . $vals['addr_floor'] . 'F';
    }

    if($vals['addr_room']){
      $output .= '-' . $vals['addr_room'];
    }
    return $output;
  }
  
  function submitOrder($form_state){
    $rec = new stdClass();
    $rec->body = serialize($form_state['storage']['vals']);
    drupal_write_record('yaucjp_order', $rec);
  }
}

class JpZipQuery
{
  static protected function mb_strtr($str,$map,$enc){
    $out="";
    $strLn=mb_strlen($str,$enc);
    $maxKeyLn=1;
    foreach($map as $key=>$val){
      $keyLn=mb_strlen($key,$enc);
      if($keyLn>$maxKeyLn){
        $maxKeyLn=$keyLn;
      }
    }
    for($offset=0; $offset<$strLn; ){
      for($ln=$maxKeyLn; $ln>=1; $ln--){
        $cmp=mb_substr($str,$offset,$ln,$enc);
        if(isset($map[$cmp])){
          $out.=$map[$cmp];
          $offset+=$ln;
          continue 2;
        }
      }
      $out.=mb_substr($str,$offset,1,$enc);
      $offset++;
    }
    return $out;
  }
  
  static protected function normalizeZip($zip){
    $zip = preg_replace('/[- ー－　（）]/u', '', $zip);
    $zip = $this->mb_strtr(
      $zip,
      array(
        '０' => '0',
        '１' => '1',
        '２' => '2',
        '３' => '3',
        '４' => '4',
        '５' => '5',
        '６' => '6',
        '７' => '7',
        '８' => '8',
        '９' => '9',
      ), 'utf8'
    );
    
    return $zip;
  }
  
  static public function query($zip){
    $db = new mysqli('localhost', 'jpzip', 'jpzip', 'jpzip');
    $db->query('SET NAMES "utf8"');
    $rs = $db->query('SELECT * FROM zip WHERE zip="' . $zip . '"');
    
    $recs = array();
    while($rec = $rs->fetch_assoc()){
      $recs[] = $rec;
    }

    return $recs;
  }
}

class YaucjpTable extends \Gulei\Table\Select
{
  function __construct($arch_src, $param = array()){
    parent::__construct($arch_src, null, $param);
  }
  
  function getRecords($recs, $param = array()){
    $recs = parent::getRecords($recs, $param);
    if($recs){ return $recs; }
    
    $rs = db_query('
      SELECT * FROM {yaucjp_order}
    ');
    
    $recs = array();
    while($data = db_fetch_object($rs)){
      $recs[] = unserialize($data->body);
    }  

    return $recs;
  }
}

?>