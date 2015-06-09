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

          $BinTest = new \Gulei\Util\BinTest();

          foreach($func['src'] as $src){
            $break = false;

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

            if($src->getAttribute('or')){
              $BinTest->testOr($chk);
            }
            else{
              $BinTest->testAnd($chk);
            }

            $test = $BinTest->getOrVal() || $BinTest->getAndVal();
            if($BinTest->getOrVal()){ $break = true; }

            $msg = empty($func['msg'])
              ? null
              : implode("<br/>", $func['msg']);

            switch($func['type']){
              case 'sep':
                if($ObjTable->isSeparatorSet() || !$test){
                  $ObjTable->markRecord($name, $ind, true, 'sep');
                  $ObjTable->markRecord($name, $ind, $msg, 'error');
                }
                break;

              default:
                if(!$test){
                  $ObjTable->markRecord($name, $ind, $msg, $func['type']);
                }
            }

            if($break){
              break;
            }
          }
        }
    }
  }
}