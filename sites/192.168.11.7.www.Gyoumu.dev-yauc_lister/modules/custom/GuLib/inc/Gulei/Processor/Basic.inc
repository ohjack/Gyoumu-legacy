<?php
namespace Gulei\Processor;

class Basic extends \Gulei\Reference\Wrapper\_Abstract
{
  protected $dp;
  protected $mods = array();

  public function applyArchReference($arch)
  {
    $ArchParam = new \Gulei\Arch\Param($arch);
    $this->ref->add($ArchParam->getReference());
    $this->ref->add(array('_arch' => $arch));
    return $this;
  }

  public function addModifier(\Gulei\Modifier\_Abstract $mod)
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

  public function setDataProvider(\Gulei\Provider\_Interface $dp)
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