<?php
namespace Listing\_Array\Command\Amazon;

class getSearchKeywords extends \Gulei\_Array\Command\_Abstract
{
  const START = 0;
  const MIN_LENGTH = 3;
  const GROUP_SIZE = 30;
  const HIT_RATE = 3;

  private function isKeyIncluded($newKey, &$keys)
  {
    foreach($keys as &$key){
      if(strpos($key, $newKey) === 0){
        $key = $newKey;
        return true;
      }
      if(strpos($newKey, $key) === 0){
        return true;
      }
    }
    return false;
  }

//  private function splitKeyGroup($skus, $size = self::MIN_LENGTH)
  private function splitKeyGroup($skus, $size = 1)
  {
    $skus = (array)$skus;
    $src = implode("\n", $skus);
    $keys = array();

    foreach($skus as $sku){
      $found = false;

      for($len = strlen($sku); $len >= self::MIN_LENGTH; $len--){
        $sub = substr($sku, 0, $len);
        $pat = sprintf("/(^%s.*|\\n%s.*)/", $sub, $sub);
        preg_match_all($pat, $src, $m);

        if(count($m[0]) >= self::HIT_RATE){
//          if(in_array($sub, $keys, true)){
          if($this->isKeyIncluded($sub, $keys)){
            $found = true;
            break;
          }

          $keys[] = $sub;
          $found = true;
          break;
        }
      }

      if(!$found){
        $keys[] = $sku;
      }
    }
    $keys = array_values($keys);
    return $keys;
  }

  private function refineKeyGroup($key_group)
  {
    $split = false;

    foreach($key_group as $key => $group){
      if(count($group) > self::GROUP_SIZE){
        $split = true;
        $sub_group = $this->splitKeyGroup($group, strlen($key) + 1);
        $key_group = array_merge($key_group, $sub_group);
        unset($key_group[$key]);
      }
    }
    return $split ? $this->refineKeyGroup($key_group) : $key_group;
  }

  public function execute($arg = null)
  {
    $key_group = $this->splitKeyGroup($this->ArryObj);
    $key_group = $this->refineKeyGroup($key_group);
    return $key_group;
  }
}