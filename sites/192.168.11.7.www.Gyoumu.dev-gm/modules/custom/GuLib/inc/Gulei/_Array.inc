<?php
namespace Gulei;

class _Array extends \ArrayObject
{
  static public function lambda()
  {
    $args = func_get_args();

    $classname = __CLASS__;
    $Ary = new $classname((array)array_shift($args));
    return $Ary->exe(array_shift($args), $args);
  }

  private function fixClassname($name)
  {
    if(strpos($name, "\\") === false){
      return __CLASS__ . "\\Command\\" . $name;
    }
    return $name;
  }

  public function exe($name, $args = null)
  {
    $name = $this->fixClassname($name);
    $cmd = new $name($this);
    return $cmd->execute($args);
  }
}