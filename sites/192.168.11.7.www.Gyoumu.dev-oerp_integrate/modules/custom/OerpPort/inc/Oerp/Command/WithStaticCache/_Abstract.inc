<?php
namespace Oerp\Command\WithStaticCache;

abstract class _Abstract extends \Gulei\Command\WithReference\_Abstract
{
  static protected $cache = array();

  abstract protected function getCacheId();
  abstract protected function setCache();

  protected function setCacheInternal($data)
  {
    self::$cache[$this->getCacheId()] = $data;
  }

  protected function getCache()
  {
    $cid = $this->getCacheId();
    if($cache = self::$cache[$cid]){
      return $cache;
    }
    return null;
  }
}