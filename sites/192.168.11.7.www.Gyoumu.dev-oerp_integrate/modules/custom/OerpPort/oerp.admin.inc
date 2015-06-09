<?php

function oerp_settings_form(){
  $form = array();
  
  $form['server'] = array(
    '#type' => 'textfield',
    '#title' => 'Server',
    '#default_value' => variable_get('oerpport_server', ''),
  );
  
  $form['db'] = array(
    '#type' => 'textfield',
    '#title' => 'Database',
    '#default_value' => variable_get('oerpport_db', ''),
  );
  
  $form['uname'] = array(
    '#type' => 'textfield',
    '#title' => 'Username',
    '#default_value' => variable_get('oerpport_uname', ''),
  );
  
  $form['pass'] = array(
    '#type' => 'password',
    '#title' => 'Password',
    '#default_value' => variable_get('oerpport_pass', ''),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

function oerp_settings_form_submit($form, &$form_state){
  //drupal_set_message('<pre>' . print_r($form_state, true) . '</pre>');
  variable_set('oerpport_server', $form_state['values']['server']);
  variable_set('oerpport_db', $form_state['values']['db']);
  variable_set('oerpport_uname', $form_state['values']['uname']);
  variable_set('oerpport_pass', $form_state['values']['pass']);  
}