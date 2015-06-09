<?php
namespace Gulei\Csv;

module_load_include('php', 'GuLib', 'lib/DataSource');
module_load_include('php', 'GuLib', 'lib/PHPTAL');

class Model extends \File_CSV_DataSource
{
  function __construct($param){
    if(is_string($param)){
      parent::__construct($param);
    }
    else if(is_array($param)){
      if($param['filepath']){
        parent::__construct($param['filepath']);
      }
      else if($param['source']){
        $fp = file_save_data($param['source'], file_directory_temp() . '/GuCsv_src.csv');
        parent::__construct($fp);
      }
    }
  }

  public function dump(){
    $Arry = new \Gulei\_Array\Basic($this->getRawArray());
    return $Arry->dumpCsv();
  }

  public function setCellVal($val, $row_num, $col_name){
    $col_key = array_search($col_name, $this->getHeaders());

    if($col_key !== false){
      $this->rows[$row_num][$col_key] = $val;
    }
  }

  public function getArchSrc(){
    $tpl = new \PHPTAL(drupal_get_path('module', 'GuLib') . '/include/xml/GuCsv_ArchSrcTpl.xml');
    $tpl->cols = $this->getHeaders();
    return $tpl->execute();
  }

  static public function getUploadEle($name = 'csv_file'){
    $form = array();

    $form[$name] = array(
      '#name' => 'files[' . $name . ']',
      '#type' => 'file',
      '#title' => 'Select a CSV file',
      '#size' => '',
    );

    $form['#attributes']['enctype'] = 'multipart/form-data';
    return $form;
  }

  static public function validateFileUpload(&$form_state, $fieldName = 'csv_file'){
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
        form_set_error($fieldName, 'Non-CSV file provided.');
        return false;
      }

      // set files to form_state, to process when form is submitted
      $form_state['values'][$fieldName] = $file;
      return true;
    }
    else {
      // set error
      form_set_error($fieldName, 'Error uploading file.');
      return false;
    }
  }
}
