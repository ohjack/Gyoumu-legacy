<?php
namespace Gulei\Arch;

module_load_include('php', 'GuLib', 'lib/PHPTAL');

class Template
{
  public $t;

  function __construct($arch, $state, $params = array()){
    $arch = preg_replace(
      '/tal:condition="state:(.*)"/U',
      'tal:condition="exists:state/\1"',
      $arch
    );

    $fp = file_save_data($arch, file_directory_temp() . '/GuArchTpl.xml');
    $this->t = new \PHPTAL($fp);
    $this->t->setOutputMode(\PHPTAL::XML);

    $this->init($state, $params);
  }

  protected function init($state, $params){
    $t = $this->t;

    $param_send = new \stdClass();
    foreach($params as $key => $val){
      $param_send->$key = $val;
    }

    $t->param = $param_send;

    if(!is_null($state)){
      $state_send = new \stdClass();
      $state_send->$state = true;

      $t->state = $state_send;
    }
  }

  public function getArch(){
    return $this->t->execute();
  }
}