<?php

class YaucPageActive extends FormPageController
{
  public function hook_form(&$form_state, $sellerId)
  {
    $form = array();

    $form['searchpanel'] = array(
      '#type' => 'fieldset',
      '#attributes' => array('id' => 'searchpanel'),
    );

    $form['searchpanel']['auc_id'] = array(
      '#title' => 'A.ID',
      '#type' => 'textfield',
      '#size' => null,
      '#default_value' => $_GET['auc_id'],
    );

    $form['searchpanel']['sku'] = array(
      '#title' => 'SKU',
      '#type' => 'textfield',
      '#size' => null,
      '#default_value' => $_GET['sku'],
    );

    $form['searchpanel']['title'] = array(
      '#title' => 'Title',
      '#type' => 'textfield',
      '#size' => null,
      '#default_value' => $_GET['title'],
    );

    $form['searchpanel']['submit:search'] = array(
      '#type' => 'submit',
      '#value' => 'Search',
      '#name' => 'submit:search',
      '#prefix' => '<div style="clear:both"></div>',
    );

    $form['searchpanel']['submit:clear'] = array(
      '#type' => 'submit',
      '#value' => 'Clear',
      '#name' => 'submit:clear',
    );

    $t = new YaucActiveTable($sellerId);

    $form['list'] = $t->getFormEles();
    $form['list']['#prefix'] = '<h1>' . $sellerId . '</h1>';

    $form['toolbar-bottom'] = array(
      '#type' => 'fieldset',
      '#attributes' => array('id' => 'toolbar-bottom'),
    );

    $form['toolbar-bottom']['submit:toEnd'] = array(
      '#type' => 'submit',
      '#name' => 'submit:toEnd',
      '#value' => 'End Listing',
    );

    $form['toolbar-bottom']['submit:toKill'] = array(
      '#type' => 'submit',
      '#name' => 'submit:toKill',
      '#value' => 'Kill Listing',
    );
    
    $form['toolbar-bottom']['submit:cancelAction'] = array(
      '#type' => 'submit',
      '#name' => 'submit:cancelAction',
      '#value' => 'Cancel Action',
    );

    jquery_ui_add('ui.datepicker');
    drupal_add_css(drupal_get_path('module', 'yaucapi') . '/yaucapi.css');
    drupal_add_js(drupal_get_path('module', 'yaucapi') . '/yaucapi.js');
    return $form;
  }

  private function flagSelection($flag, $flag_val)
  {
    foreach($this->getSelection() as $sel){
      $CtrlFlag = new YaucFlagController($sel);
      $CtrlFlag->setFlag($flag, $flag_val);
    }
  }

  protected function proc_searchpanel_submit_search()
  {
    $this->goBack(false);
  }

  protected function proc_searchpanel_submit_clear()
  {
    $this->goBack();
  }

  protected function proc_toolbar_bottom_submit_toEnd()
  {
    $this->flagSelection('to_end', true);
    $this->flagSelection('to_kill', false);
    $this->goBack(false);
  }

  protected function proc_toolbar_bottom_submit_cancelAction()
  {
    $this->flagSelection('to_end', false);
    $this->flagSelection('to_kill', false);
    $this->goBack(false);
  }

  protected function proc_toolbar_bottom_submit_toKill()
  {
    $this->flagSelection('to_end', true);
    $this->flagSelection('to_kill', YAPI_FLAG_TOKILL);
    $this->goBack(false);
  }
}

class YaucPageRelist extends FormPageController
{
  public function hook_form(&$form_state, $sellerId){
    $form = $this->getFormTemplate();
    $t = new YaucRelistTable($sellerId, array('type' => 'select-many'));

    $form['sellerId'] = array(
      '#type' => 'value',
      '#value' => $sellerId,
    );

    $form['relist']['list'] = $t->getFormEles();
    $form['relist']['list']['#prefix'] = '<h2>Relist List - ' . $sellerId . '</h2>';
    $form['relist']['func:action'] = array(
      '#type' => 'select',
      '#options' => array(
        'ignore' => 'Ignore selected item(s)',
      ),
      '#default_value' => 'ignore',
    );

    $form['relist']['func:run'] = array(
      '#type' => 'submit',
      '#name' => 'func:fun',
      '#value' => 'Run',
    );

    $t = new YaucIgnoreTable($sellerId);

    $form['ignores']['list'] = $t->getFormEles();
    $form['ignores']['list']['#prefix'] = '<h2>Ignores</h2><div style="width:800px;">';
    $form['ignores']['list']['#suffix'] = '</div>';

    $form['ignores']['func:action'] = array(
      '#type' => 'select',
      '#options' => array(
        'restore' => 'Restore selected item(s)',
        'kill' => 'Kill selected item(s)',
        'cancel' => 'Cancel all actions'
      ),
      '#default_value' => 'restore',
    );

    $form['ignores']['func:run'] = array(
      '#type' => 'submit',
      '#value' => 'Run',
      '#name' => 'submit:run',
    );

    $yapi = new GuYapi($sellerId);
    $yapi->checkAccessToken(true);

    $yauc = new GuYauc($sellerId);
    $yauc->checkDriverReady(true);

    return $form;
  }

  private function ignoreOrNot($sels, $cond)
  {
    foreach($sels as $auc_id){
      $rec = db_fetch_object(
        db_query('SELECT status, seller_id FROM {yaucapi_auction} WHERE auc_id="%s"', $auc_id));

      $newRec = new stdClass();
      $newRec->auc_id = $auc_id;

      if($cond) {
        switch ($rec->status) {
          case YAPI_AUC_SOLD:
            $newRec->status = YAPI_AUC_SOLD_IGNORE;
            drupal_write_record('yaucapi_auction', $newRec, 'auc_id');
            break;

          case YAPI_AUC_CLOSE:
            $newRec->status = YAPI_AUC_CLOSE_IGNORE;
            drupal_write_record('yaucapi_auction', $newRec, 'auc_id');
            break;
        }
      }
      else {
        $FlagCtrl = new YaucFlagController($auc_id);
        $FlagCtrl->setFlag('to_kill', false);

        switch ($rec->status) {
          case YAPI_AUC_SOLD_IGNORE:
            $newRec->status = YAPI_AUC_SOLD;
            drupal_write_record('yaucapi_auction', $newRec, 'auc_id');
            break;

          case YAPI_AUC_CLOSE_IGNORE:
            $newRec->status = YAPI_AUC_CLOSE;
            drupal_write_record('yaucapi_auction', $newRec, 'auc_id');
            break;
        }
      }
    }
  }

  private function killOrNot($sels, $cond)
  {
    foreach($sels as $sel){
      $FlagCtrl = new YaucFlagController($sel);
      $flags = $FlagCtrl->getAucFlags();

      if($cond){
        if($flags->to_kill == YAPI_FLAG_TOKILL){
          $FlagCtrl->setFlag('to_kill', YAPI_FLAG_KILLING_SCHEDULED);
        }
        else{
          $FlagCtrl->setFlag('to_kill', YAPI_FLAG_KILLING);
        }
      }
      else{
        if($flags->to_kill == YAPI_FLAG_KILLING){
          $FlagCtrl->setFlag('to_kill', false);
        }
        else{
          $FlagCtrl->setFlag('to_kill', YAPI_FLAG_TOKILL);
        }
      }
    }
    $this->goBack();
  }

  protected function proc_relist_func_run()
  {
    $act = $this->getStateValues($this->getClickedButtonArrayParents(1, 'func:action'));
    $sels = $this->getSelection($this->getClickedButtonArrayParents(1, 'list'));

    switch($act){
      case 'ignore':
        $this->ignoreOrNot($sels, true);
        break;
    }
    $this->goBack();
  }

  protected function proc_ignores_func_run()
  {
    $act = $this->getStateValues($this->getClickedButtonArrayParents(1, 'func:action'));
    $sels = $this->getSelection($this->getClickedButtonArrayParents(1, 'list'));

    switch($act){
      case 'restore':
        $this->ignoreOrNot($sels, false);
        break;

      case 'kill':
        $this->killOrNot($sels, true);
        break;

      case 'cancel':
        $this->killOrNot($sels, false);
        break;
    }
    $this->goBack();
  }

  protected function proc_download($form, &$form_state)
  {
    $recs = array();
    foreach($form['list']['#recs'] as $ind => $rec){
      if($form_state['values']['sel-' . $ind]){
        preg_match('/href="([^"]*)"/', $rec['auc_url'], $m);
        $rec['auc_url'] = $m[1];

        $recs[] = $rec;
      }
    }

    $Recs = new GuArray($recs);

    $arch = file_get_contents(drupal_get_path('module', 'yaucapi') . '/xml/macroRelist.xml');
    $params = array(
      'urls' => $Recs->collect('auc_url')
    );

    $tpl = new GuArchTemplate($arch, null, $params);
    $fp = file_save_data($tpl->getArch(), file_directory_temp() . '/YaucJp_Relist-' . date('Y-m-d') . '.iim');
    $form_state['storage']['file'] = $fp;
  }
}

class YaucPageSold extends FormPageController
{
  public function hook_form(&$form_state, $sellerId)
  {
    $this->redirect($form_state);

    $output = '<h1>' . $sellerId . '</h1>';
    $t = new YaucSoldTable($sellerId, array('search' => $form_state['storage']['search']));

    $output .= $t->theme(null);

    $form = array();

    $form['searchpanel'] = array(
      '#type' => 'fieldset',
      '#attributes' => array('id' => 'searchpanel'),
    );

    $form['searchpanel']['start_time'] = array(
      '#title' => 'Start Time',
      '#type' => 'textfield',
      '#size' => null,
      '#attributes' => array('widget' => 'datepicker'),
      '#default_value' => $_GET['start_time']
    );

    $form['searchpanel']['end_time'] = array(
      '#title' => 'End Time',
      '#type' => 'textfield',
      '#size' => null,
      '#attributes' => array('widget' => 'datepicker'),
      '#default_value' => $_GET['end_time']
    );

    $form['searchpanel']['search'] = array(
      '#type' => 'submit',
      '#value' => 'Search',
      '#name' => 'search',
      '#prefix' => '<div style="clear:both"></div>',
    );

    $form['searchpanel']['clear'] = array(
      '#type' => 'submit',
      '#value' => 'Clear',
      '#name' => 'clear',
    );

    $form['list'] = array(
      '#type' => 'markup',
      '#value' => $output,
      '#obj' => $t->save(),
    );

    $form['download'] = array(
      '#type' => 'submit',
      '#value' => 'Download',
      '#name' => 'download',
    );

    jquery_ui_add('ui.datepicker');
    drupal_add_css(drupal_get_path('module', 'yaucapi') . '/yaucapi.css');
    drupal_add_js(drupal_get_path('module', 'yaucapi') . '/yaucapi.js');
    return $form;
  }

  protected function proc_download($form, &$form_state)
  {
    $funcSortDownload = create_function(
      '$a,$b',
      'return strcmp($a["time_end"], $b["time_end"]);'
    );

    $t = $form['list']['#obj']->renew();

    $form_state['storage']['redirect'] =
      $t->getCsvFile(
        array('sort' => $funcSortDownload),
        'YahooOrderRaw'
      );
  }

  protected function proc_searchpanel_search($form, &$form_state)
  {
    $this->goBack(false);
    /*
    $vals = $form_state['values'];

    $form_state['storage']['search'] = array(
      'start_time' => $vals['start_time'],
      'end_time' => $vals['end_time'],
    );
    */
  }

  protected function proc_searchpanel_clear()
  {
    $this->goBack();
  }
}