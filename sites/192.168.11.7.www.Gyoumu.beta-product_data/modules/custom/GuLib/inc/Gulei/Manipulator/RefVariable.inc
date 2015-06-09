<?php
namespace Gulei\Manipulator;

class RefVariable
{
  private $vars;

  public function addRefVar(array $input)
  {
    foreach($input as $name => &$var)
    {
      unset($this->vars[$name]);
      $this->vars[$name] = &$var;
    }
  }

  public function &refVar($name)
  {
    return $this->vars[$name];
  }

  public function refVars()
  {
    $vars = array();
    foreach(func_get_args() as $name)
    {
      $vars = &$this->vars[$name];
    }
    return $vars;
  }

  public function val($name)
  {
    return $this->vars[$name];
  }

  public function vals()
  {
    $vars = array();
    foreach(func_get_args() as $name){
      $vars[$name] = $this->vars[$name];
    }
    return $vars;
  }
}