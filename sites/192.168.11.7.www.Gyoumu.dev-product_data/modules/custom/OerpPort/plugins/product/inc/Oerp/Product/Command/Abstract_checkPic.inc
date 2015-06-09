<?php
namespace Oerp\Product\Command;

abstract class Abstract_checkPic extends \Gulei\Command\WithReference\WithTable
{
  static protected $cache = array();

  abstract protected function createFinder();
  abstract protected function getCacheId();

  private function checkCache($cache)
  {
    return $cache && $cache->data && $cache->expire > time();
  }

  public function getCache()
  {
    $cid = $this->getCacheId();

    if($cache = self::$cache[$cid]){
      return $cache;
    }

    $cache = cache_get($this->getCacheId());

    if(!$this->checkCache($cache)){
      $this->setCache();
      $cache = cache_get($this->getCacheId());

      if(!$this->checkCache($cache)){
        return null;
      }
    }
    return self::$cache[$cid] = $cache;
  }

  protected function setCache()
  {
    $Finder = $this->createFinder();
    $list = $Finder->getList();

    $skus = array();
    $pat = '@.*/?([a-z]{2}-\d{5})-.*\.jpg@i';

    foreach($list as $line){
      if(preg_match($pat, $line, $m)){
        if(!isset($skus[$m[1]])){
          $skus[$m[1]] = true;
        }
      }
    }

    $cid = $this->getCacheId();
    cache_set($cid, $skus, 'cache', time() + 60 * 15);
  }

  protected function isPicExist($sku)
  {
    $cache = $this->getCache();

    if(is_null($cache)){
      $this->setCache();
      $cache = $this->getCache();
    }

    if(!$cache){
      return false;
    }

    $sku = strtoupper(trim($sku));
    return isset($cache->data[$sku]);
  }

  public function execute($refVar = null)
  {
    list($rec) = $this->ref->vals('rec');
    $sku = $rec['default_code'];

    if($this->isPicExist($sku)){
      return 'OK';
    }
    else{
      return '---';
    }
  }
}