<?php
namespace Gulei\Arch;

class Param
{
  private $Arch;

  function __construct($arch)
  {
    $this->Arch = new \Gulei\Arch\Element($arch);
  }

  private function getParam(&$ref, $Ele, $eleName = null)
  {
    foreach($Ele->attributes as $attr){
      if($eleName){
        $ref[$eleName]['_attr'][$attr->name] = $attr->value;
      }
      else {
        $ref['_attr'][$attr->name] = $attr->value;
      }
    }

    $params = $this->Arch->xp('./param', $Ele);
    if($params->length){
      foreach($params as $param){
        $name = $param->getAttribute('name');
        $nodeValue = $param->nodeValue;

        if($eleName){
          $ref[$eleName]['_param'][$name] = $nodeValue;
        }
        else{
          $ref['_param'][$name] = $nodeValue;
        }
      }
    }
  }

  public function getReference()
  {
    $ref = array();
    $this->getParam($ref, $this->Arch->getArch());

    $Eles = $this->Arch->xp(".//field", $this->Arch->getArch());
    foreach($Eles as $Ele){
      $eleName = $Ele->getAttribute('name');
      $this->getParam($ref, $Ele, $eleName);
    }
    return $ref;
  }
}