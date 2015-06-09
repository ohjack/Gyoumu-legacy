<?php
namespace Oerp\Command;

class match extends \Gulei\Command\WithReference\WithTable
{
  public function execute($refVar = null)
  {
    list($rec, $name) = array(
      $this->ref->val('rec'),
      $this->ref->val('name'),
    );

    $args = $this->getCommandArgs();
    $cri = ($cri_src = json_decode($args[1])) ? $cri_src : array();
    $flds = ($cri_src = json_decode($args[2])) ? $cri_src : array('id');

    foreach($cri as &$stat){
      if(preg_match('/\$\{(.*)\}/', $stat[2], $m)){
        $stat = str_replace($m[0], $rec[$m[1]], $stat);
      }
    }

    $q = new \Oerp\Query\Basic($args[0], $cri, $flds);
    $ret = (array)$q->raw();

    if($func_src = $args[3]){
      $func = create_function('$ref,$refVar,$ret', $func_src);
      return call_user_func($func, $this->ref, $refVar, $ret);
    }
    return $ret;
  }
}