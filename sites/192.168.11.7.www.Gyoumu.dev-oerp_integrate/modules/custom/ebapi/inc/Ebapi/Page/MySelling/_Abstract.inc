<?php
namespace Ebapi\Page\MySelling;

abstract class _Abstract extends \Gulei\Page\Form\Basic
{
  protected $accnt;
  abstract protected function getArch();
  abstract protected function getAhahPath();

  protected $PAGE_ITEMS = 40;
  protected $resp;

  protected function getSearchSku()
  {
    if(!($sku = $_GET['search-sku'])){
      $sku = $this->form_state['values']['search-sku'];
    }
    return $sku;
  }

  protected function getSearchSkuArray()
  {
    $str = $this->getSearchSku();
    $skus = explode("\n", $str);
    $skus = array_map('trim', $skus);
    $skus = array_filter($skus);
    return $skus;
  }

  protected function getRecs()
  {
    $Account = new \Ebapi\Account\Helper($this->accnt);
    $ebay = $Account->getQuery();

    $param = array(
      'EndTimeFrom' => \eBayDateHelper::getDate('now -5minutes'),
      'EndTimeTo' => \eBayDateHelper::getDate('now +31days'),
      'Pagination' => array(
        'EntriesPerPage' => $this->PAGE_ITEMS,
      ),
    );

    if($skus = $this->getSearchSkuArray()){
      $param['SKUArray'] = array(
        'SKU' => $skus,
      );
    }

    $list = $ebay->GetSellerList($param);
    $this->resp = $list->toArray();

    if($items = $this->resp['ItemArray']['Item']){
      if(isset($items['ItemID'])){
        $items = array($items);
      }
    }
    else{
      $items = array();
    }

    $recs = array();
    foreach($items as $item){
      $recs[] = array(
        'ItemID' => $item['ItemID'],
        'Title' => l(
          $item['Title'], $item['ListingDetails']['ViewItemURL'],
          array('attributes' => array('target' => '_blank'))
        ),
        'SKU' => $item['SKU'],
        'Site' => $item['Site'],
        'StartTime' => $item['ListingDetails']['StartTime'],
        'ListingType' => $item['ListingType'],
        'BuyItNowPrice' => $item['BuyItNowPrice'],
        'CurrentPrice' => $item['SellingStatus']['CurrentPrice'],
        'MinimumToBid' => $item['SellingStatus']['MinimumToBid'],
        'ShippingServiceCost' => $item['ShippingDetails']['ShippingServiceOptions']['ShippingServiceCost'],
        'QuantityAvailable' => $item['QuantityAvailable'],
        'Quantity' => $item['Quantity'],
      );
    }

    return $recs;
  }

  private function getEleAhah($param){
    $ele = array_merge(
      array(
           '#ahah' => array(
//             'path' => 'ebapi/myebay/selling/js',
             'path' => $this->getAhahPath(),
             'wrapper' => 'ebapi-listings-wrapper',
           ),
      ),
      $param
    );
    return $ele;
  }

  protected function getEleSearch()
  {
    $ele = array();

    $ele['searchpanel-search'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div class="ebapi-searchpanel-search">',
      '#suffix' => '</div>',
    );

    $ele['searchpanel-search']['search-sku'] = array(
      '#title' => 'SKUs',
      '#type' => 'textarea',
      '#description' => 'Sepreate SKUs each line.',
      '#default_value' => ($dval = $this->getSearchSku()) ? $dval : '',
    );

    $ele['searchpanel-buttons'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div class="ebapi-searchpanel-buttons">',
      '#suffix' => '</div>',
    );

    $ele['searchpanel-buttons']['search-search'] = $this->getEleAhah(
      array(
           '#type' => 'submit',
           '#value' => 'Search',
           '#name' => 'submit',
      )
    );

    $ele['searchpanel-buttons']['search-clear'] = $this->getEleAhah(
      array(
           '#type' => 'submit',
           '#value' => 'Clear',
           '#name' => 'submit',
      )
    );

    return $ele;
  }

  protected function getEleList()
  {
    $arch = $this->getArch();
    $t = new ListingTable($arch, $this->getRecs(), array('type' => 'select-many'));
    $ele = $t->getFormEles();
    $ele['#prefix'] = sprintf('<div>%s<div id="ebapi-listings-list">', $this->getExceedingMessage());
    $ele['#suffix'] = '</div></div>';
    return $ele;
  }

  private function getExceedingMessage()
  {
    if($this->resp['PaginationResult']['TotalNumberOfPages'] > 1){
      return '<div id="exc_warning">Exceeds limitation of items per page: ' . $this->PAGE_ITEMS . '. Remaining items are hidden.</div>';
    }
    return '';
  }

  public function hook_form(&$form_state, $accnt)
  {
    $this->accnt = $accnt;
    $form = array();

    $form['wrapper'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div id="ebapi-listings-wrapper">',
      '#suffix' => '</div>',
    );

    $form['wrapper']['accnt'] = array(
      '#type' => 'hidden',
      '#value' => $accnt,
    );

    $form['wrapper']['search'] = $this->getEleSearch();
    $form['wrapper']['list'] = $this->getEleList();
    return $form;
  }

  public function hook_form_js($form_id)
  {
    $form_state = array('values' => $_POST);
    $this->form_state = $form_state;

    if($form_state['values']['submit'] == 'Clear'){
      $this->form_state = $form_state = array();
    }

    $form = $this->hook_form($form_state, $_POST['accnt']);
    drupal_process_form($form_id, $form, $form_state);

    unset($form['wrapper']['#prefix'], $form['wrapper']['#suffix']);
    $output = theme('status_messages') . drupal_render($form['wrapper']);
    drupal_json(array('status' => TRUE, 'data' => $output));
  }

  protected function proc_wrapper_search_search_search($form, &$form_state)
  {
    $this->goBack(false);
  }

  protected function proc_wrapper_search_search_clear($form, &$form_state)
  {
    $this->goBack();
  }
}