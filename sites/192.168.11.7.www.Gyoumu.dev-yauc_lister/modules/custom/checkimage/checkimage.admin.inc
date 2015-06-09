<?php

function checkimage_settings_path_form(&$form_state)
{
  $form = array();

  $form['local_paths'] = array(
    '#type' => 'textarea',
    '#title' => 'Local Paths',
    '#default_value' => variable_get('checkimage:local_paths', '')
  );

  $form['pm_paths'] = array(
    '#type' => 'textarea',
    '#title' => 'Paths on picturemaster.info',
    '#default_value' => variable_get('checkimage:pm_paths', '')
  );

  $form['rt_paths'] = array(
    '#type' => 'textarea',
    '#title' => 'Paths on Rakuten Cabinet',
    '#default_value' => variable_get('checkimage:rt_paths', '')
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

function checkimage_settings_path_form_submit($form, &$form_state)
{
  $vals = $form_state['values'];
  variable_set('checkimage:local_paths', $vals['local_paths']);
  variable_set('checkimage:pm_paths', $vals['pm_paths']);
  variable_set('checkimage:rt_paths', $vals['rt_paths']);

  drupal_set_message('Settings saved.');
}

?>