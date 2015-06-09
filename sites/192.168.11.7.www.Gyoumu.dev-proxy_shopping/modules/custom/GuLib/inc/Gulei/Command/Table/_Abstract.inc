<?php
namespace Gulei\Command\Table;

abstract class _Abstract extends \Gulei\Command\WithReference\WithTable
{
  public function createCommand($ref)
  {
    $cmd_name = $ref['src']->getAttribute('name');
    preg_match('/^cmd:(.*)/', $cmd_name, $m);

    if($cmd_classname = $m[1]){
      if(strpos($cmd_classname, "\\") === false){
        $cmd_classname = "Gulei\\Command\\" . $cmd_classname;
        return new $cmd_classname($ref);
      }
      return new $cmd_classname($ref);
    }
    return new \Gulei\Command\UserFunction($ref);
  }

  protected function getRootName($name) {
    $root_name = explode('.', $name);
    array_pop($root_name);

    $root_name = count($root_name)
        ? implode('.', $root_name) . '.' : '';

    return $root_name;
  }
}