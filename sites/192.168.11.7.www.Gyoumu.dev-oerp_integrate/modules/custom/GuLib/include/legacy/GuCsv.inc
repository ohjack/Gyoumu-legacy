<?php

function GuCsv_form_validate($form, &$form_state, $fieldName){
  // If a file was uploaded, process it.
  if (isset($_FILES['files']) && is_uploaded_file($_FILES['files']['tmp_name'][$fieldName])) {
    // attempt to save the uploaded file
    $file = file_save_upload($fieldName);

    // set error if file was not uploaded
    if (!$file) {
      form_set_error($fieldName, 'Error uploading file.');
      return false;
    }

    // check whether CSV
    if($file->filemime != 'text/csv'){
      form_set_error($fieldName, 'Please provide a CSV file.');
      return false;
    }
       
    // set files to form_state, to process when form is submitted
    $form_state['values']['file'] = $file;
    return true;
  }
  else {
    // set error
    form_set_error($fieldName, 'Error uploading file.');
    return false;   
  }  
}

function GuLib_GuCsv_process($ele, $edit, &$form_state, $complete_form){
  $b = new \Gulei\Csv\Buffer($ele['#arch'], $form_state);
  $ele = array_merge($ele, $b->getFormEntry($form_state));
  
  $ele['#obj'] = $b->save();
  return $ele;
}

function GuLib_GuCsv_validate($ele, &$form_state){
  if($form_state['clicked_button']['#name'] !== 'upload'){
    return;
  }

  $obj = $ele['#obj']->renew();
  $fieldName = $obj->attr('name');
  
  // If a file was uploaded, process it.
  if (isset($_FILES['files'])
    && is_uploaded_file($_FILES['files']['tmp_name'][$fieldName])) {
      
    // attempt to save the uploaded file
    $file = file_save_upload($fieldName);

    // set error if file was not uploaded
    if (!$file) {
      form_set_error($fieldName, 'Error uploading file.');
      return false;
    }

    // check whether CSV
    if($file->filemime != 'text/csv'){
      form_set_error($fieldName, 'Please provide a CSV file.');
      return false;
    }
       
    // set files to form_state, to process when form is submitted
    $val = &$form_state['values'];
    foreach($ele['csv_file']['#parents'] as $lv){
      $tmp = &$val[$lv];
      unset($ele);
      $val = &$tmp;
      unset($tmp);
    }
    $val = $file;          
    return true;
  }
  else {
    // set error
    form_set_error($fieldName, 'Error uploading file.');
    return false;   
  }
}

function theme_GuCsv($form){
  return $form['#children'];
}

class GuTestArchImporter implements InterfaceArchImporter
{
  public function getArch()
  {
    return <<<XML
  <tree>
    <field name="SKU"/>
    <field name="Name"/>
  </tree>
XML;
  }

  public function submit($form, &$form_state)
  {
    $buf = $form['csv_file']['#obj']->renew();
    dpr($buf->getRecords()); die;
  }
}