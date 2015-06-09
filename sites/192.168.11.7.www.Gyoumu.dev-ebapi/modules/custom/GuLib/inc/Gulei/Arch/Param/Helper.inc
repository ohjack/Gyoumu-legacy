<?php
namespace Gulei\Arch\Param;

class Helper extends \Gulei\Arch\Element
{
  protected $param;

  function __construct($arch, $param = array()){
    parent::__construct($arch);
    $this->param = $param;
  }

  protected function getDefaultParam(){
    return array();
  }

  public function getParam($name = null, $override = null){
    $param = array_merge(
      $this->getDefaultParam(),
      $this->getNodeValues('./param[@name]')
    );

    $param = array_merge($param, $this->param);

    if(is_array($override)){
      $param = array_merge($param, $override);
    }

    if(is_null($name)){
      return $param;
    }
    else{
      return $param[$name];
    }
  }

  public function xp($query, $context = null, $attr_map = null){
    $xp = new \DOMXPath($this->getDoc());

//  register Namespaces
    if($this->param['ns']){
      foreach($this->param['ns'] as $prefix => $uri){
        $xp->registerNamespace($prefix, $uri);
      }
    }

    return parent::xp($query, $context, $attr_map, $xp);
  }
}