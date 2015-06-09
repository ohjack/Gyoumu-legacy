<?php
namespace Oerp\Installer;

abstract class _Abstract
{
  protected $param;

  abstract protected function getTablename();
  abstract protected function getParamMapping();
  abstract protected function getDefaultParam();
  abstract protected function getDefCri();
  abstract protected function update($def);

  public function __construct()
  {
    $param = func_get_args();

    if(count($param) == 1 && is_array($param[0])){
      $this->param = array_merge($this->getDefaultParam() , $param);
    }
    else{
      $mapping = $this->getParamMapping();

      for($i = 0; $i < count($mapping) && $i < count($param); $i++){
        $this->param[$mapping[$i]] = $param[$i];
        $this->param = array_merge($this->getDefaultParam(), $this->param);
      }
    }
    $this->init();
  }

  protected function init()
  {
    return null;
  }

  public function getParam()
  {
    return $this->param;
  }

  protected function createQuery($cri = null, $fld = null, $ids = null)
  {
    $q = new \Oerp\Query\Basic($this->getTablename(), $cri, $fld, $ids);
    return $q;
  }

  protected function getDef()
  {
    $cri = $this->getDefCri();
    $resp = $this->createQuery($cri)->raw();
    return $resp[0];
  }

  public function exe()
  {
    if($def = $this->getDef()){
      $this->update($def);
      return true;
    }

    $q = $this->createQuery();
    return $q->create($this->param);
  }
}