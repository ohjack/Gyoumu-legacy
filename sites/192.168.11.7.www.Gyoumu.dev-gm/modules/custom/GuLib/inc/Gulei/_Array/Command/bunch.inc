<?php
namespace Gulei\_Array\Command;

class bunch extends \Gulei\_Array\Command\_Abstract
{
  public function execute($args = null)
  {
    list($key, $DROPKEY, $inner_key) = $args;

    $DROPKEY = ($DROPKEY === null) ? false : $DROPKEY;
    $inner_key = ($inner_key === null) ? 0 : $inner_key;

    $newArry = array();
    foreach($this->ArryObj as $item){
      if(!empty($item[$key])){
        $ind = (is_array($item[$key])) ? $item[$key][$inner_key] : $item[$key];

        if($DROPKEY){
          unset($item[$key]);
        }

        if(!isset($newArray[$ind])){
          $newArray[$ind] = array();
        }
        $newArry[$ind][] = $item;
      }
    }
    return $newArry;
  }
}