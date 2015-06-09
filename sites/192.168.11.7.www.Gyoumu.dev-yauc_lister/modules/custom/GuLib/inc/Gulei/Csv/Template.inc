<?php
namespace Gulei\Csv;

class Template extends \Gulei\Table\Csv
{
  protected $input;
  protected $output;

  function __construct($param){
    $this->input = new \Gulei\Csv\Model($param['input']);

    if($param['output']){
      $this->output = new \Gulei\Csv\Model($param['output']);
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