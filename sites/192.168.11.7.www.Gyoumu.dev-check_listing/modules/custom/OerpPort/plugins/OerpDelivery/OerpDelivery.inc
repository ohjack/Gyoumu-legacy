<?php

module_load_include('inc', 'oerp', '/oerp');
module_load_include('inc', 'GuLib', '/include/GuTime');

class OerpDeliveryList extends OerpArchForm
{
  private $ele_name;
  private $state;
  private $ids = '';
  private $params;
  
  private static $tree = array();
  private static $form = array();
  
  function __construct($state, $ids = null, $params = array()){
    $this->state = $state;
    $this->ids = $ids;
    $this->params = $params;
        
    parent::__construct($this->getTemplate(null, $params));
  }

  public function getFormEntry($name){
    $this->ele_name = $name;
    $form = parent::getFormEntry($name);
    $form['#attributes']['class'] = 'oerp-formview';
    $form['#attributes']['enctype'] = 'multipart/form-data';
    return $form;
  }
  
  public function process(&$form_state, $form){
    if($form_state['storage']['combine']){
      $state = 'combine';
    }
    else if($form_state['storage']['cancel']){
      $state = 'canceling';
    }
    else{
      $state = $this->state;
    }

    $this->processModel($state, $form_state, $form);
  }
  
  protected function processModel($state, &$form_state, $form){
    $Fld = new OerpRecord($this->dump());

    if(!empty($form_state['values']['order_list']['select:order_id'])){
      $vals = $Fld->adaptValues($form_state['values']['order_list']);
      $sels = $vals['select:order_id'];
      
      if(!empty($sels)){
        $sels = array_map('intval', $sels);
      }
    }

    $btn = $form_state['clicked_button']['#name'];
    $q = new OerpQuery($this->attr('model'));
        
    switch($state){
      case 'assigned':
      case 'confirmed':
      case 'draft':
      case 'done':
      case 'cancel':
        switch($btn){
          case 'func:combine':
            if(count($sels) > 1){
              $form_state['storage']['combine'] = $sels;
            }
            break;
            
          case 'func:assign':
            $this->forceAvail($sels);
            break;
            
          case 'func:send':
            $q->execute('draft_validate', $sels);
            break;
            
          case 'func:delete':
            $this->delDraft($sels);
            break;
            
          case 'func:cancel':
            $form_state['storage']['cancel'] = $sels;
//            $this->cancelOrders($sels);
            break;
            
          case 'func:printFedex':
            $obj = $form[$this->ele_name]['select:order_id']['#obj']->renew();
            
            if(!$sels){
              $recs = $obj->getRecordsAltered(array(
                'filter' => '
                  <filter>
                    <function>
                      <source><![CDATA[
                        return empty($rec[$name])
                          && strpos($rec["carrier_id"], "Fedex") === 0;
                      ]]></source>
    
                      <field>x_tnum</field>
                    </function>

                    <func>
                      <src name="func:beforeCloseTime"/>
                      <fld>date</fld>
                    </func>
                  </filter>
                ',
              ));

              $Recs = new GuArray($recs);
              $sels = $Recs->collect('id');
              unset($Recs, $recs);
            }

            $sels = array_filter($sels);
            $sels = array_filter($sels, 'is_numeric');
            $sels = array_values($sels);

            if($sels){
              $this->printFedex($sels);
              drupal_goto('fedprint/overview/record');
            }
            else{
              drupal_set_message('No record printed.');
            }

            break;

          case 'func:printUps':
            $obj = $form[$this->ele_name]['select:order_id']['#obj']->renew();

            if(!$sels){
              $recs = $obj->getRecordsAltered(array(
                'filter' => '
                  <filter>
                    <function>
                      <source><![CDATA[
                        return empty($rec[$name]) && $rec["carrier_id"] === "UPS";
                      ]]></source>

                      <field>x_tnum</field>
                    </function>

                    <func>
                      <src name="func:beforeCloseTime"/>
                      <fld>date</fld>
                    </func>
                  </filter>
                ',
              ));

              $Recs = new GuArray($recs);
              $sels = $Recs->collect('id');
              unset($Recs, $recs);
            }

            $sels = array_filter($sels);
            $sels = array_filter($sels, 'is_numeric');
            $sels = array_values($sels);

            if($sels){
              self::printUps($sels);
            }
            else{
              drupal_set_message('No record printed.');
            }

            break;

          case 'func:download-notrack':
            $arch = $this->getTemplate(
              $this->state,
              array(
                'showFullAddr' => true,
                'search' => $this->params['search'], 
              )
            );
            
            $ele = new OerpArchElement($arch);
            $arch = $ele->dump($ele->xp('/form/tree[@name="select:order_id"]')->item(0));
            unset($ele);

            $filter = <<<XML
<filter>
  <func>
    <src name="func:isEmpty"/>
    <fld>x_tnum</fld>
  </func>
  <func>
    <src name="func:beforeCloseTime"/>
    <fld>date</fld>
  </func>
</filter>

XML;

            $obj = new OerpTable($arch,
              array(
                'filter' => $filter,
              )
            );

            $form_state['storage']['redirect'] =
              './' . $obj->getCsvFile(null, 'DeliveryOrdersReady');
                                                  
            break;
            
          case 'func:download-japan':
            $arch = $this->getTemplate('done', array('search' => $this->params['search']));
            $ele = new OerpArchElement($arch);
            $arch = $ele->dump($ele->xp('/form/tree[@name="select:order_id"]')->item(0));
            unset($ele);
            
            $obj = new OerpTable($arch,
              array(
                'filter' => '
                  <filter>
                    <func>
                      <src><![CDATA[
                        return $rec[$name] == "Japan";
                      ]]></src>
                      <fld>address_id.state_id.country_id</fld>
                    </func>
                  </filter>
                ',
              )
            );

            $form_state['storage']['redirect'] =
              './' . $obj->getCsvFile(null, 'DeliveryOrdersJapan_sent');

            break;

          case 'func:download-these':
            $arch = $this->getTemplate('done', array('search' => $this->params['search']));
            $ele = new OerpArchElement($arch);
            $arch = $ele->dump($ele->xp('/form/tree[@name="select:order_id"]')->item(0));
            unset($ele);
            
            $obj = new OerpTable($arch);

            $form_state['storage']['redirect'] =
              './' . $obj->getCsvFile(null, 'DeliveryOrdersThese_sent');
                                                  
            break;
            
          case 'func:cancelAvail':
            $this->cancelAvail($sels);
            break;
        }
        return;
        
      case 'combine':
        switch($btn){
          case 'func:combine':
            $t = $form[$this->ele_name]['select:order_id']['#obj'];
            $this->combineOrders($this->ids, $sels['sel']);
            break;
            
          case 'func:cancel':
            break;
        }
        unset($form_state['storage']);
        return;
        
      case 'canceling':
        switch($btn){
          case 'func:confirm':
            $note = trim($form_state['values']['order_list']['func:note']);

            if(empty($note)){
              drupal_set_message('Please enter cancel note.', 'error');
              return;
            }

            $ids = $this->ids;
            foreach($ids as $id){
              $q->setIds($id);
              $q->setFields(array('note'));

              $note_send = $q->raw();
              $note_send = $note_send[0]['note'];
              
              $note_send .= empty($note_send)
                ? "CANCELED: $note" : "\nCANCELED: $note";

              $q->write(array('note' => $note_send));
            }

            $this->cancelOrders($this->ids);
            break;
            
          case 'func:cancel':
            break;
        }
        unset($form_state['storage']);
        return;
    }
  }

  protected function printFedex($rids){
    $arch = file_get_contents(
      drupal_get_path('module', 'OerpDelivery') . '/xml/printFedex.xml');
      
    $mapping = '
      <mapping>
        <field name="id" from="id"/>
        <field name="tid" from="name"/>
        <field name="recp_email" from="address_id.email"/>
        <field name="recp_contact" from="address_id.name"/>
        <field name="recp_phone" from="address_id.phone"/>
        <field name="recp_addr1" from="address_id.street"/>
        <field name="recp_addr2" from="address_id.street2"/>
        <field name="recp_city" from="address_id.city"/>
        <field name="recp_post_code" from="address_id.zip"/>
        <field name="recp_state" from="address_id.state_id.name"/>
        <field name="recp_country_code" from="address_id.state_id.country_id.code"/>
        <field name="ref_note" from="origin"/>
        <field name="pkg_type" from="func:pkg_type"/>
        <field name="adm_pkg_type" from="func:adm_pkg_type"/>
        <field name="pkg_weight" from="weight"/>
        <field name="customs_value" from="x_cust_val"/>
        <field name="service_type" from="carrier_id"/>
        <field name="com_desc1" from="func:com_desc1"/>
      </mapping>
      ';

    $param = array(
      'oerp:ids' => json_encode($rids)
    );

    $q = new OerpTable($arch, $param);
    $recs = $q->mapRecords($mapping);

    foreach($recs as $rec){
      $q = new OerpQuery('stock.picking', null, array('x_tnum'), $rec['id']);
      $resp = $q->raw();

      _fedprint_delRec($resp[0]['x_tnum'], 'tnum');
      $q->write(array('x_tnum' => '_PRINT_'));

      $srv = $rec['service_type'];
      
//    select service type  
      if($srv == 'Fedex IE'){
        $rec['service_type'] = 3;
      }
      else if($srv == 'Fedex IP'){
        $rec['service_type'] = 1;
      }
      else{
        continue;
      }

      $form_state = array('values' => $rec);
      drupal_execute(
        'fedprint_RecordCreate_form', $form_state, 0);
    }
  }

  static public function printUps($rids){
    $arch = GuFile::getPathContent('module', 'OerpDelivery', '/xml/printFedex.xml');

    $mapping = '
      <mapping>
        <field name="id" from="id"/>
        <field name="tid" from="name"/>
        <field name="recp_email" from="address_id.email"/>
        <field name="recp_contact" from="address_id.name"/>
        <field name="recp_phone" from="address_id.phone"/>
        <field name="recp_addr1" from="address_id.street"/>
        <field name="recp_addr2" from="address_id.street2"/>
        <field name="recp_city" from="address_id.city"/>
        <field name="recp_post_code" from="address_id.zip"/>
        <field name="recp_state" from="address_id.state_id.name"/>
        <field name="recp_country_code" from="address_id.state_id.country_id.code"/>
        <field name="ref_note" from="origin"/>
        <field name="pkg_weight" from="weight"/>
        <field name="custom_value" from="x_cust_val"/>
        <field name="service_type" from="carrier_id"/>
        <field name="com_desc1" from="func:com_desc1"/>
      </mapping>
      ';

    $param = array(
      'oerp:ids' => json_encode($rids)
    );

    $q = new OerpTable($arch, $param);
    $recs = $q->mapRecords($mapping);

    foreach($recs as $rec){
      $q = new OerpQuery('stock.picking', null, array('x_tnum'), $rec['id']);
      $q->write(array('x_tnum' => '_PRINT_'));

      $UpsCtrl = new OerpUpsPrintController($rec['tid']);
      $UpsCtrl->sendRecord($rec);
    }
  }

  protected function combineOrders($sels, $main){
    $q = new OerpQuery(
      $this->attr('model'), null, array('note', 'move_lines', 'x_cust_val'));
      
    $sels = array_filter($sels, create_function('$var', "
      return \$var !== $main; 
    "));
    
//  prepare main rec
    $main_q = new OerpQuery(
      $this->attr('model'), null, array('x_cust_val'), $main);

    $main_rec = (array)$main_q->raw();
    $main_rec = $main_rec[0];
    unset($main_rec['id']);

//  combine move_lines  
    foreach($sels as $sel){
      $q->setIds($sel);
      $rec = (array)$q->raw();
      $rec = $rec[0];
      unset($rec['id']);
      
      $main_rec['x_cust_val'] += $rec['x_cust_val'];
/*      
      $rec['note'] .= empty($rec['note']) 
        ? "COMBINED"
        : "\nCOMBINED";
*/       
      foreach($rec['move_lines'] as &$line){
        $line = array(
          OERP_WRITE_EDIT,
          $line,
          array('picking_id' => $main),
        );
      }

      $q->write($rec);
    }
    
    $main_q->write($main_rec);
    
    foreach($sels as $sel){
      $q->setIds($sel);
      $q->unlink();
    }
//    $this->cancelOrders($sels);
  }
  
  protected function cancelOrders($ids){
    $q = new OerpQuery($this->attr('model'), null, array('origin'));
    
    foreach($ids as $id){
      $q->setIds($id);
      $rec = (array)$q->raw();
      $rec = $rec[0];
      
      unset($rec['id']);
      $rec['date_done'] = date('Y-m-d H:i:s');
      
      $q->write($rec);
      $q->exec_workflow(array('button_cancel', $id));
      
      _fedprint_delRec($rec['name'], 'tid');
    }

    drupal_set_message(sprintf('%d record(s) cancelled.', count($ids)));
  }
  
  protected function forceAvail($ids){
    $q = new OerpQuery($this->attr('model'));
    
    if($this->state == 'draft'){
      $q->execute('draft_force_assign', $ids);
    }
    $q->execute('force_assign', $ids);    
  }
  
  protected function cancelAvail($ids){
    $q = new OerpQuery(
      $this->attr('model'), null, array('origin'));
    
    foreach($ids as $id){
      $q->setIds($id);

//    clear fedprint record  
      $rec = $q->raw();
      _fedprint_delRec($rec[0]['name'], 'tid');

      $q->write(array('x_tnum' => ''));
      $q->execute('cancel_assign', array($id));
    }
  }
  
  protected function delDraft($ids){
//  del movelines first  
    $q = new OerpQuery('stock.move', null, array('id'));
    foreach($ids as $id){
      $q->setCriteria(sprintf('[["picking_id", "=", %s]]', $id));
      $toDel = $q->raw()->collect('id');
      $q->execute('unlink', $toDel);
    }
    
//  then pickings  
    $q = new OerpQuery($this->attr('model'));
    $q->execute('unlink', $ids);    
  }
  
  protected function getTemplate($state = null, $param_over = null){
    $param_over = empty($param_over) ? array() : $param_over;
    
//  default params  
    $params = array_merge(
      array(
        'type' => 'select-many',
        'action' => 'select',
        'model' => 'stock.picking',
        'ids' => json_encode($this->ids),
        'showTrackNum' => false,
        'showNote' => false,
        'showFullAddr' => false,
        'showDateDone' => false,
        'page' => $_GET['page'],
        'items_per_page' => 30,
      ),
      $param_over
    );
    
    $state = is_null($state) ? $this->state : $state;
    
//  set params
    switch($state){
      case 'confirm':
        $params['showFullAddr'] = true;
        break;
        
      case 'draft':
        break;
        
      case 'assigned':
        $params['showTrackNum'] = true;
        break;
        
      case 'done':
        if($search = $params['search']){
          if($search == '*'){
            $params['criteria'] = '[["type", "=", "delivery"],["state", "=", "done"]]';
          }
          else{
            $cri = array();
  
            foreach($params['search'] as $name => $keyword){
              $keyword = trim($keyword);
              
              preg_match('/search-(.*):(.*)/', $name, $m);
              list($dummy, $op, $field) = $m;
              
              $params['search-val'][$field . '-' . $op] = $keyword;
             
              if(strlen($keyword)){
                $cri[] = json_encode(
                  array($field, OerpFunc_SearchOpMap($op), $keyword)
                );
              }
            }
            
            $params['criteria'] = sprintf(
              '[["type", "=", "delivery"],["state", "=", "done"],%s]', implode(',', $cri)
            );
          }
        }
        else{
          $params['criteria'] = sprintf(
            '[["type", "=", "delivery"],["state", "=", "done"],["date_done", ">=", "%s"]]', date('Y-m-d'));
        }
        
        $params['showTrackNum'] = true;
        $params['showDateDone'] = true;
        break;
        
      case 'confirmed':
        $params['showTrackNum'] = true;
        break;
        
      case 'cancel':
        $params['type'] = '';
        $params['showNote'] = true;
        $params['showDateDone'] = true;
        break;
        
      case 'combine':
        $params['type'] = 'select-one';
        $params['showFullAddr'] = true; 
        break;
        
      case 'canceling':
        $params['type'] = '';
        break;
        
      case '*':
        $params['items_per_page'] = 0;
        break;
    }

    $arch = file_get_contents(drupal_get_path('module', 'OerpDelivery') . '/xml/DeliveryList.xml');
    $tpl = new GuArchTemplate($arch, $state, $params);

    return $tpl->getArch();  
  }
  
  function backupAll($state, $tpl_param = array(), $tb_param = array()){
    $tpl_param = array_merge(array(
      'showFullAddr' => true,
      'showTrackNum' => true,
    ), $tpl_param);
    
    $arch = $this->getTemplate($state, $tpl_param);

    $ele = new OerpArchElement($arch);
    $arch = $ele->dump($ele->xp('/form/tree[@name="select:order_id"]')->item(0));
    unset($ele);
    
    $tb_param = array_merge(array(
      'copy_method' => 'copy',
      'nl2br' => true,
    ), $tb_param);
    
    $obj = new OerpTable($arch, $tb_param);
    $name = 'DeliveryOrder_All';
    
//    $path = sprintf('%s/%s_%s.csv', file_directory_temp(), $name, date('Y-m-d'));
    $path = sprintf('%s/%s.csv', file_directory_temp(), $name);
    $src = $obj->dumpCsv();
    file_put_contents($path, $src);
    
    return $obj->dumpCsv();
  }
}

class OerpDeliveryModel
{
  static private $DeliveryOrderCreate;
  
  public function getEles_DeliveryOrderCreate(){
    if(self::$DeliveryOrderCreate){
      return self::$DeliveryOrderCreate->getFormEntry();
    }
    else{
      $arch = file_get_contents(
        drupal_get_path('module', 'OerpDelivery') . '/xml/DeliveryOrderCreate.xml');
        
      self::$DeliveryOrderCreate = new OerpArchForm($arch); 
      return self::$DeliveryOrderCreate->getFormEntry();
    }
  }
}

module_load_include('inc', 'UpsPrint', '/UpsPrint');

class OerpUpsPrintController extends UpsPrintController
{
  function __construct($id)
  {
    parent::__construct(
      '/home/jeremy/UpsListener/in',
      '/home/jeremy/UpsListener/out',
      $id
    );
  }
}

class OerpDeliveryOrderRecord extends OerpRecord
{
  private $carrier;

  public function getRecord($param = array())
  {
    $rec = parent::getRecord($param);
    $this->carrier = $rec['carrier_id']['name'];

    $UpsPrintCtrl = new OerpUpsPrintController($rec['name']);

    if(!$rec['x_tnum'] && $UpsPrintCtrl->isPrint()){
      if($msg = $UpsPrintCtrl->getLogMessage()){
        drupal_set_message(nl2br($msg), 'error', false);
      }
    }
    return $rec;
  }

  public function getFormEntry()
  {
    $form = array();
    $form['details'] = parent::getFormEntry();
    $form['details']['#width'] = 'large';

    $form['submit:save'] = array(
      '#type' => 'submit',
      '#value' => 'Save',
      '#name' => 'submit:save',
    );

    if($this->carrier == 'UPS'){
      $form['submit:save_printUps'] = array(
        '#type' => 'submit',
        '#value' => 'Save & Print',
        '#name' => 'submit:save_printUps',
      );
    }

    $form['RecObj'] = array(
      '#type' => 'value',
      '#value' => $this->save(),
    );

    return $form;
  }
}

?>
