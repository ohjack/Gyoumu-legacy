<?php
namespace Gulei\Csv;

class Buffer extends \Gulei\Table\Csv
{
  function __construct($arch, &$form_state, $param = array()){
    if($file = $form_state['values']['csv_file']){
      $param['filepath'] = $file->filepath;
      parent::__construct($arch, $param);
    }
    else{
      parent::__construct($arch, $param);
    }
  }

  public function getFormEntry(&$form_state){
    $param = $this->getParam();
    $form = parent::getFormEntry($form_state);
    $form['#attributes']['enctype'] = 'multipart/form-data';

    $form['upload'] = array(
      '#type' => 'submit',
      '#value' => 'Upload',
      '#name' => 'upload',
    );

    if($param['filepath']){
      $overview = '';

      if($this->xp('./translate | ./validate', $this->getArch())->length){
        $overview .= '
          <div class="gu-legend">
            <div><span class="invalid">RED</span> = Invalid</div>
            <div><span class="notice">BROWN</span> = Notice</div>
            <div><span class="translated">GREEN</span> = Modified</div>
          </div>
      ';
      }

      $overview .= $this->theme();

      $form['overview'] = array(
        '#type' => 'markup',
        '#value' => $overview,
      );

      if($this->isValid()){
        $form['submit'] = array(
          '#type' => 'submit',
          '#value' => 'Submit',
          '#name' => 'submit',
        );
      }else{
        drupal_set_message('Correct invalid fields and upload again.', 'error');
      }
    }
    return $form;
  }

  protected function validateCsv(&$form_state, $fieldName = 'csv_file'){
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
      $form_state['values']['csv_file'] = $file;
//      $this->opts['filepath'] = $file->filepath;
      return true;
    }
    else {
      // set error
      form_set_error($fieldName, 'Error uploading file.');
      return false;
    }
  }

  public function validateForm(&$form_state){
    $btn = $form_state['clicked_button']['#name'];

    if($btn == 'upload' && $this->validateCsv($form_state)){
      $form_state['rebuild'] = true;
    }
  }

  function theme($ele = null){
    return parent::theme($ele);
  }
}