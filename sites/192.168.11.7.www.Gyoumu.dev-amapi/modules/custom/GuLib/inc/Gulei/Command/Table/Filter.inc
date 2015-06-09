<?php
namespace Gulei\Command\Table;

class Filter extends _Abstract
{
  public function execute($refVar = null)
  {
    $func = $this->ref->val('func');
    $ObjTable = $this->ref->val('ObjTable');

    $recs = &$refVar->refVar('recs');
    $prevChk = false;

    foreach($func['src'] as $src){
      foreach($func['fld'] as $name){
        foreach($recs as $ind => $rec){
          if($rec[$name] === '_DITTO_'){
            if(!$prevChk){
              unset($recs[$ind]);
            }
            continue;
          }

          $ref = array(
            'src' => $src,
            'recs'=> $recs,
            'ind' => $ind,
            'rec' => $rec,
            'name' => $name,
            'prefix' => $this->ref->val('prefix'),
            'ObjTable' => $ObjTable,
          );
          $chk = $this->createCommand($ref)->execute(new \Gulei\Manipulator\RefVariable());

          if(!$chk){
            unset($recs[$ind]);
            $prevChk = false;
          }
          else{
            $prevChk = true;
          }
        }
      }
    }
  }
}