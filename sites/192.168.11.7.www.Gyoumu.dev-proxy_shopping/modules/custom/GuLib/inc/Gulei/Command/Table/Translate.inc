<?php
namespace Gulei\Command\Table;

class Translate extends _Abstract
{
  public function execute($refVar = null)
  {
    $param = $this->ref->val('param');
    $func =  $this->ref->val('func');
    $ObjTable = $this->ref->val('ObjTable');

    $recs = &$refVar->refVar('recs');

    foreach($func['src'] as $src){
      foreach($func['fld'] as $name){
        foreach($recs as $ind => &$rec){
          if(
            (!array_key_exists($name, $rec) && !preg_match('(^func:|.*\.func:)', $name))
            || $rec[$name] === '_DITTO_'){
            continue;
          }

          $ref = array(
            'src' => $src,
            'recs'=> $recs,
            'ind' => $ind,
            'rec' => $rec,
            'name' => $name,
            'root_name' => $this->getRootName($name),
            'prefix' => $this->ref->val('prefix'),
            'ObjTable' => $ObjTable,
          );

          $newVal = $this->createCommand($ref)->execute(new \Gulei\Manipulator\RefVariable());

          if($newVal === $rec[$name]){
            $newVal = false;
          }

          if($newVal !== false){
            $rec[$name] = $newVal;

            if($param['mark_translated']){
              $msg = empty($func['msg'])
                ? null
                : implode("<br/>", $func['msg']);

              $ObjTable->markRecord($name, $ind, $msg, 'translated');
            }
          }
        }
      }
    }
  }
}

