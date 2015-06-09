<?php
namespace Gulei\Command\Table;

class Validate extends _Abstract
{
  public function execute($refVar = null)
  {
    $param = $this->ref->val('param');
    $func = $this->ref->val('func');
    $ObjTable = $this->ref->val('ObjTable');

    $recs = $refVar->val('recs');

    foreach($recs as $ind => &$rec){
        foreach($func['fld'] as $name){
          if(!isset($rec[$name]) && $param['mark_missing']){
            $rec[$name] = '*MISSING*';
            $ObjTable->markRecord($name, $ind, 'Missing Column: ' . $name, 'error');
          }
          if($rec[$name] === '_DITTO_'){
            continue;
          }


          for($i = 0 ; $i < count($func['src']); $i++){
            $src = $func['src'][$i];
            $left = count($func['src']) - $i;

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
            $chk = $src->getAttribute('not') ? !$chk : $chk;

            if($src->getAttribute('or')){
              if($chk){
                break;
              }
              else if($left > 1){
                continue;
              }
            }

            if(!$chk || $ObjTable->isSeparatorSet()){
              $msg = empty($func['msg'])
                  ? null
                  : implode("<br/>", $func['msg']);

              switch($func['type']){
                case 'sep':
                  $ObjTable->markRecord($name, $ind, true, 'sep');
                  $ObjTable->markRecord($name, $ind, $msg, 'error');
                  break;

                default:
                  $ObjTable->markRecord($name, $ind, $msg, $func['type']);
                  break;
              }
            }
          }
        }
    }
  }
}