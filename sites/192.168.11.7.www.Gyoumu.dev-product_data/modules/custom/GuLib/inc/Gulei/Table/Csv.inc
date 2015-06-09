<?php
namespace Gulei\Table;

class Csv extends Select
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
    $c = new \Gulei\Provider\Csv($this->getParam('filepath'));
    $this->recs = $c->getData();
    return $this->recs;
  }
}