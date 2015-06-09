<?php
namespace Gulei\_Array\Command;

class mergeRecursively extends _Abstract
{
  public function execute($newArry = null)
  {
    $arrays = array(
      (array)$this->ArryObj,
      $newArry
    );

    $merged = array();
    while ($arrays) {
      $array = array_shift($arrays);
      if (!is_array($array)) {
        trigger_error(__FUNCTION__ .' encountered a non array argument', E_USER_WARNING);
        return;
      }
      if (!$array)
        continue;
      foreach ($array as $key => $value)
        if (is_string($key))
          if (is_array($value) && array_key_exists($key, $merged) && is_array($merged[$key])){
            $classname = __CLASS__;
            $Cmd = new $classname($merged[$key]);
            $merged[$key] = $Cmd->execute($value);
          }
          else{
            $merged[$key] = $value;
          }
        else{
          $merged[] = $value;
        }
    }
    return $merged;
  }
}