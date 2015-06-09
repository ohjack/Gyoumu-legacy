<?php
namespace Yauc\Page\Listings;

class Active extends \Gulei\Page\Form\Basic
{
  public function hook_form(&$form_state, $sellerId)
  {
    $this->redirect($form_state);
    $form = array();

    $form['sellerId'] = array(
      '#type' => 'value',
      '#value' => $sellerId,
    );

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

    $t = new \Yauc\Table\Listings\Active($sellerId);

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

    $form['toolbar-bottom']['submit:downloadPricelist'] = array(
      '#type' => 'submit',
      '#name' => 'submit:downloadPricelist',
      '#value' => 'Download Pricelist',
    );

    jquery_ui_add('ui.datepicker');
    drupal_add_css(drupal_get_path('module', 'yaucapi') . '/yaucapi.css');
    drupal_add_js(drupal_get_path('module', 'yaucapi') . '/yaucapi.js');
    return $form;
  }

  private function flagSelection($flag, $flag_val)
  {
    foreach($this->getSelection() as $sel){
      $CtrlFlag = new \YaucFlagController($sel);
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

  protected function proc_toolbar_bottom_submit_downloadPricelist($form, &$form_state)
  {
    $vals = $this->getValues();
    $prov = new \Yauc\Provider\Listings\Active($vals['sellerId']);
    $Recs = new \Gulei\_Array\Basic($prov->getData());

    $mapping = <<<XML
<tree>
  <field name="SKU" from="sku"/>
  <field name="Title" from="title"/>
  <field name="Init.Price" from="init_price"/>
  <field name="Cur.Price" from="cur_price"/>
  <field name="S.Fee" from="shipping_fee"/>
  <field name="Carrier" from="carrier"/>
</tree>
XML;

    $mapped = $Recs->mapping($mapping);
    array_unshift($mapped, array_keys($mapped[0]));

    $Recs = new \Gulei\_Array\Basic($mapped);
    $csv_src = $Recs->dumpCsv();

    $path = sprintf('%s/%s_%s_%s.csv', file_directory_temp(), 'ActivePricelist', $vals['sellerId'], date('Ymd'));
    $fp = '.' . file_save_data($csv_src, $path);
    $form_state['storage']['redirect'] = $fp;
  }
}