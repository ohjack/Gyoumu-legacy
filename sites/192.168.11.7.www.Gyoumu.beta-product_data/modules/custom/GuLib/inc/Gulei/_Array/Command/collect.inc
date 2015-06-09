<?php
namespace Gulei\_Array\Command;

class collect extends _Abstract
{
  public function execute($args = null)
  {
    $key = $args[0];
    $newArry = array();

    foreach($this->ArryObj as $item){
      if(is_array($item[$key])){
        foreach($item[$key] as $each){
          $newArry[] = $each;
        }
      }else{
        $newArry[] = $item[$key];
      }
    }
    return array_values(array_unique($newArry));
  }
}