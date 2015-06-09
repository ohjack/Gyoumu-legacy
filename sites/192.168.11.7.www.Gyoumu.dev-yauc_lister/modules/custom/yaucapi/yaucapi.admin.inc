<?php
function yaucapi_form_seller(&$form_state, $seller_id = ''){
  if($seller_id){
    $Seller = new GuYapi($seller_id);
  }
  
  $form['seller_id'] = array(
    '#type' => 'textfield',
    '#title' => 'Seller ID',
    '#default_value' => ($Seller) ? $Seller->getSellerId() : '',
    '#description' => 'Yahoo Auction Seller ID.',
    '#required' => TRUE,
  );

  $form['password'] = array(
    '#type' => 'password',
    '#title' => 'Password',
  );

  $form['relistAnchor'] = array(
    '#type' => 'textfield',
    '#title' => 'Relist Anchor',
    '#default_value' => ($Seller) ? $Seller->getRelistAnchor() : '',
    '#description' => 'Anchor character for relisting.',
  );

  $form['appid'] = array(
    '#type' => 'textfield',
    '#title' => 'AppID',
    '#default_value' => ($Seller) ? $Seller->getAppID() : '',
  	'#description' => 'AppID obtained from Yahoo Developer Network.',
    '#required' => TRUE,
  );
  
  $form['consumer_key'] = array(
    '#type' => 'textfield',
    '#title' => 'Consumer Key',
    '#default_value' => ($Seller) ? ($val = $Seller->getConsumerKey()) ? $val : '' : '',
    '#description' => 'OAuth-enabled Consumer Key.', 
  );

  $form['consumer_secret'] = array(
    '#type' => 'textfield',
    '#title' => 'Consumer Secret',
    '#default_value' => ($Seller) ? ($val = $Seller->getConsumerSecret()) ? $val : '' : '',
  );  
  
  $submit_caption = ($seller_id) ? 'Save' : 'Add'; 
  $form['submit'] = array('#type' => 'submit', '#value' => t($submit_caption));
  $form['#redirect'] = 'admin/settings/yaucapi';
  
  return $form;
}

function yaucapi_form_seller_submit($form, &$form_state){
  $vals = $form_state['values'];
  if(strlen($vals['password']) < 1){
    unset($vals['password']);
  }

  switch($form_state['values']['op']){
    case t('Save'):
      $result = drupal_write_record('yaucapi_sellers',
        $vals, 'seller_id');
      break;
    case t('Add'):
      $result = drupal_write_record('yaucapi_sellers',
        $vals);
      break;
  }

  switch($result){
    case FALSE:
      drupal_set_message('Database error occurs.', 'error');
      break;
    case SAVED_NEW:
      drupal_set_message("Seller '{$form_state['values']['seller_id']}' added.");
      break;
    case SAVED_UPDATED:
      drupal_set_message("Seller '{$form_state['values']['seller_id']}' updated.");
      break;
  }

  return;
}

function yaucapi_seller_confirm_delete(&$form_state, $seller_id){
  $form = array();  
  $form['seller_id'] = array('#type' => 'value', '#value' => $seller_id);
  $form['#redirect'] = 'admin/settings/yaucapi';
  return confirm_form($form,
  	"Are you sure you want to delete '$seller_id'?",
    'admin/settings/yaucapi',
    '',
    'Delete', 'Cancel');
}

function yaucapi_seller_confirm_delete_submit($form, &$form_state){
  db_query('DELETE FROM {yaucapi_sellers} WHERE seller_id = \'%s\'',
    $form_state['values']['seller_id']);
    
  return SAVED_DELETED;  
}

function yaucapi_list(){
  $sellers = _yaucapi_get_sellers();
  
  $hds = array(
    array('data' => 'Seller ID'),
    array('data' => 'Operation', 'colspan' => 2),
  );

  $rows = array();
  foreach($sellers as $seller){
    $row = array();
    $row['seller_id'] = $seller->seller_id;
    $row['edit'] =
      l('edit', "admin/settings/yaucapi/seller/edit/$seller->seller_id");
      
    $row['delete'] = 
    	l('delete', "admin/settings/yaucapi/seller/delete/$seller->seller_id");
    	
  	$rows[] = $row;
  }

  if(empty($rows)){
    return 'No sellers available.';
  }
  
  return theme_table($hds, $rows, array('style' => 'width: 400px;'));
}
?>
