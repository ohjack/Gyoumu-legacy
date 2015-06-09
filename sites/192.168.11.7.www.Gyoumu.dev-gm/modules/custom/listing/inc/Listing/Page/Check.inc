<?php
namespace Listing\Page;

class Check extends \Gulei\Page\Form\Basic
{
  protected function getResult()
  {
    $vals = $this->getValues();
    $sku_src = $vals['skus'];

    $chks = array(
      new \Listing\Checker\Amazon($sku_src),
      new \Listing\Checker\YahooJp($sku_src),
      new \Listing\Checker\Rakuten($sku_src),
    );

    $recs = array();
    foreach($chks[0]->getSkus() as $sku){
      $rec = array();
      $rec['sku'] = $sku;

      foreach($chks as $chk)
      {
        $rec[$chk->getKey()] = $chk->getResult($sku);
      }
      $recs[] = $rec;
    }

    $hds = array();
    $hds['sku'] = 'SKU';
    foreach($chks as $chk) {
      $hds[$chk->getKey()] = $chk->getHeader();
    }

    $t = new \Gulei\Table\Simple($hds, $recs);

    $form['result'] = array(
      '#type' => 'markup',
      '#value' => $t->theme(),
    );

    drupal_add_js(drupal_get_path('module', 'listing') . '/inc/check-online.js');
    return $form;
  }

  public function hook_form(&$form_state)
  {
    $form = array();

    $form['skus'] = array(
      '#title' => 'SKUs',
      '#type' => 'textarea',
      '#description' => 'Enter SKUs each line.',
      '#default_value' => $this->getDefaultValue('skus'),
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Check',
    );

    if(isset($this->form_state['values']['skus'])){
      $form = array_merge($form, $this->getResult());
    }
    return $form;
  }

  protected function proc_submit($form, &$form_state)
  {
    $form_state['rebuild'] = true;
  }
}