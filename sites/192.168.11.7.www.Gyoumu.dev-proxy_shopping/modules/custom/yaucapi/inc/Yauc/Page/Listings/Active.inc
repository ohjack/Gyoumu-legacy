<?php
namespace Yauc\Page\Listings;

class Active extends \Gulei\Page\Form\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    return null;
  }

  protected function getToolbarBottom()
  {
    $calls = array('End', 'Kill', 'Cancel', 'Download');
    return $this->getLocalCalleeEle($calls);
  }

  public function hook_form_internal()
  {
    $this->redirect($this->form_state);
    $form = array();

    list($sellerId) = $this->args;

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

    jquery_ui_add('ui.datepicker');
    drupal_add_css(drupal_get_path('module', 'yaucapi') . '/yaucapi.css');
    drupal_add_js(drupal_get_path('module', 'yaucapi') . '/yaucapi.js');
    return $form;
  }

  protected function proc_searchpanel_submit_search()
  {
    $this->goBack(false);
  }

  protected function proc_searchpanel_submit_clear()
  {
    $this->goBack();
  }
}