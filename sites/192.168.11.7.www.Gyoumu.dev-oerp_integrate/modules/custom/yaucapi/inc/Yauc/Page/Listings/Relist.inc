<?php
namespace Yauc\Page\Listings;

class Relist extends \Gulei\Page\Form\Basic
{
  public function hook_form(&$form_state, $sellerId){
    $form = $this->getFormTemplate();
    $t = new \Yauc\Table\Listings\Relist($sellerId, array('type' => 'select-many'));

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

    $t = new \Yauc\Table\Listings\Ignore($sellerId);

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

    $yapi = new \GuYapi($sellerId);
    $yapi->checkAccessToken(true);

    $yauc = new \GuYauc($sellerId);
    $yauc->checkDriverReady(true);

    return $form;
  }

  private function ignoreOrNot($sels, $cond)
  {
    foreach($sels as $auc_id){
      $rec = db_fetch_object(
        db_query('SELECT status, seller_id FROM {yaucapi_auction} WHERE auc_id="%s"', $auc_id));

      $newRec = new \stdClass();
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

    $Recs = new \Gulei\_Array\Basic($recs);

    $arch = file_get_contents(drupal_get_path('module', 'yaucapi') . '/xml/macroRelist.xml');
    $params = array(
      'urls' => $Recs->collect('auc_url')
    );

    $tpl = new \Gulei\Arch\Template($arch, null, $params);
    $fp = file_save_data($tpl->getArch(), file_directory_temp() . '/YaucJp_Relist-' . date('Y-m-d') . '.iim');
    $form_state['storage']['file'] = $fp;
  }
}