<?php
use Gulei as G;

require_once 'util.inc';
require_once 'GuArch.inc';
require_once 'command.inc';
require_once 'modifier.inc';
require_once 'provider.inc';

class GuArchParam
{
  private $Arch;

  function __construct($arch)
  {
    $this->Arch = new GuArchElement($arch);
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

class GuProcessor extends G\AbstractReferenceWrapper
{
  protected $dp;
  protected $mods = array();

  public function applyArchReference($arch)
  {
    $ArchParam = new GuArchParam($arch);
    $this->ref->add($ArchParam->getReference());
    return $this;
  }

  public function addModifier(G\AbstractModifier $mod)
  {
    $this->mods[] = $mod;
    return $this;
  }

  public function addModifiers(array $mods)
  {
    foreach($mods as $mod){
      $this->addModifier($mod);
    }
    return $this;
  }

  protected function applyModifiers($recs)
  {
    foreach($this->mods as $mod){
      $mod->apply($recs, $this->ref);
    }
    return $recs;
  }

  public function setDataProvider(G\InterfaceProvider $dp)
  {
    $this->dp = $dp;
    $this->addReference($dp->getReference());
    return $this;
  }

  public function getOutput()
  {
    $recs = $this->dp->getData();
    $recs = $this->applyModifiers($recs);
    return $recs;
  }
}
