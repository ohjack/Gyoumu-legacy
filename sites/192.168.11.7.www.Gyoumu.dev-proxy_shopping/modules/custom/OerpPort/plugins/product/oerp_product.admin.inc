<?php

function oerp_product_settings_form(&$form_state)
{
  $form = array();
  
  $q = new \Oerp\Query\Basic('product.product', null, array('id', 'default_code'));

  $items = (array)$q->raw();
  $recs = \Gulei\_Array::lambda($items, "Oerp\\Product\\_Array\\Command\\sortSKU");
  $recs = array_splice($recs, -10);
  $last = array_pop($items);
  unset($items);

  $tails = '';
  foreach($recs as $rec){
    $tails .= sprintf("[%6s] %s\n", $rec['id'], $rec['default_code']);
  }
  $tails = '<pre>' . $tails . '</pre>';

  $form['tails'] = array(
    '#type' => 'markup',
    '#value' => $tails,
  );

  $offset = variable_get('oerp_product_sku_seq_offset', 0);
  
  $form['latest_sku_seq'] = array(
    '#title' => 'Latest SKU sequence',
    '#type' => 'textfield',
    '#default_value' => $last['id'] + $offset
  );

  $form['latest_id'] = array(
    '#type' => 'value',
    '#value' => $last['id'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function oerp_product_settings_form_submit($form, &$form_state)
{
  $vals = $form_state['values'];
  $offst = $vals['latest_sku_seq'] - ($vals['latest_id'] + 1);
  variable_set('oerp_product_sku_seq_offset', $offst);
}