<?php
namespace Ebapi\Csv;

class SerialLoader
{
  protected $fp;

  public function getSourceEntry()
  {
    $form = array();
    $srcFound = glob(__DIR__ . '/*.csv');

    if(!$srcFound){
      $form['msg'] = array(
        '#type' => 'markup',
        '#value' => '<strong>No source MVL found.</strong>',
      );
      return $form;
    }

    $opts = array();
    foreach($srcFound as $each){
      $toks = explode('/', $each);
      $opts[$each] = $toks[count($toks) - 1];
    }

    $form = array();
    $form['source'] = array(
      '#title' => 'MVL Source',
      '#type' => 'select',
      '#options' => $opts,
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Reload'
    );
    return $form;
  }

  public function reload($fp)
  {
    $sql = 'DELETE FROM {ebapi_fitment} WHERE 1';
    db_query($sql);

    $fh = fopen($fp, 'r');
    $hds = array();

    while(($line = fgets($fh)) !== false){
      $data = str_getcsv($line);

      if(empty($hds)){
        foreach($data as $datum){
          $hds[] = preg_replace('/[^[:alnum:]]/', '_', $datum);
        }
      }
      else{
        $rec = array_combine($hds, $data);
        drupal_write_record('ebapi_fitment', $rec);
      }
    }
  }
}