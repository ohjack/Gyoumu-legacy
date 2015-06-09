<?php

require_once __DIR__ . '/../lib/DataSource.php';
require_once __DIR__ . '/page.inc';
require_once __DIR__ . '/GuForm.inc';
require_once __DIR__ . '/provider.inc';

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

class GuCsv extends File_CSV_DataSource
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
    $Arry = new GuArray($this->getRawArray());
    return $Arry->dumpCsv();
  }
  
  public function setCellVal($val, $row_num, $col_name){
    $col_key = array_search($col_name, $this->getHeaders());
    
    if($col_key !== false){
      $this->rows[$row_num][$col_key] = $val;
    }
  }
  
  public function getArchSrc(){
    $tpl = new PHPTAL(drupal_get_path('module', 'GuLib') . '/include/xml/GuCsv_ArchSrcTpl.xml');
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

class GuCsvTable extends GuSelectTable
{
  function __construct($arch_src, $param = array()){
    parent::__construct($arch_src, null, $param);  
  }
  
  protected function getDefaultParam(){
    return array_merge(parent::getDefaultParam(), array(
      'count' => true,
    ));
  }

  function getFormEntry(&$form_state){
    $name = $this->attr('name');
    $name = $name ? $name : 'csv_file';
    
    $form = array();

    if($hdr = $this->getArch()->getAttribute('string')){
      $form['header'] = array(
        '#type' => 'markup',
        '#value' => "<h1>$hdr</h1>",
      );
    }

    $form['csv_file'] = array(
      '#name' => 'files[' . $name . ']',
      '#type' => 'file',
      '#title' => 'CSV Input',
      '#size' => '',
      '#obj' => $this->save(),
    );
    
    return $form;
  }  
  
  public function getRecords(){
    $c = new Gulei\ProviderCsv($this->getParam('filepath'));
    $this->recs = $c->getData();
    return $this->recs;
  }
}

class GuCsvBuffer extends GuCsvTable
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

function GuLib_GuCsv_process($ele, $edit, &$form_state, $complete_form){
  $b = new GuCsvBuffer($ele['#arch'], $form_state);
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

class GuCsvTemplate extends GuCsvTable
{
  protected $input;
  protected $output;
  
  function __construct($param){
    $this->input = new GuCsv($param['input']);
    
    if($param['output']){
      $this->output = new GuCsv($param['output']);
      parent::__construct($this->output->getArchSrc(), $param);      
    }
    else{
      parent::__construct($this->input->getArchSrc(), $param);
    }
  }

  protected function getDefaultParam(){
    return array_merge(
      parent::getDefaultParam(),
      array(
           'nl2br' => false,
           'sticky_header' => false,
           //      'pre' => true,
      ));
  }

  public function getRecords(){
    return $this->input->connect();
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

abstract class AbstractCsvArchImporter implements InterfaceArchImporter
{
  protected function getBufferObj($form)
  {
    return $form['csv_file']['#obj']->renew();
  }
}

class GuCsvImporter extends FormPageController
{
  public function hook_form(&$form_state, $imp_name)
  {
    $imp = new $imp_name;
    $t = new GuCsvBuffer($imp->getArch(), $form_state);

    $form = $t->getFormEntry($form_state);
    $form['#importer_classname'] = $imp_name;
    return $form;
  }

  public function hook_form_validate($form, &$form_state)
  {
    $buf = $form['csv_file']['#obj']->renew();
    $buf->validateForm($form_state);
  }

  protected function proc_submit($form, &$form_state)
  {
    $imp_name = $form['#importer_classname'];
    $imp = new $imp_name();
    return $imp->submit($form, $form_state);
  }
}