<?php
namespace Gulei\Command;

require_once 'reference.inc';

interface InterfaceCommand
{
  public function execute($refVar = null);
}

abstract class AbstractCommandWithReference extends \Gulei\AbstractReferenceWrapper implements InterfaceCommand
{
  protected function getCommandArgs()
  {
    list($ObjTable, $src) = array(
      $this->ref->val('ObjTable'),
      $this->ref->val('src')
    );
    return $ObjTable->getNodeValues('./arg[not(@name)]', $src, false);
  }

  protected function getCommandNamedArgs()
  {
    list($ObjTable, $src) = array(
      $this->ref->val('ObjTable'),
      $this->ref->val('src')
    );
    return $ObjTable->getNodeValues('./arg[@name]', $src, true);
  }
}

class UserFunction extends AbstractCommandWithReference
{
  protected function checkFuncName($src){
    if($func_name = $src->getAttribute('name')){
      preg_match('/func:(.*)/', $func_name, $m);

      if($m[1]){
        foreach($this->ref->val('prefix') as $prefix){
          if(function_exists($prefix . $m[1])){
            return $prefix . $m[1];
          }
        }
        return false;
      }
      else{
        return $func_name;
      }
    }
    else{
      return null;
    }
  }

  private function execLambdaFunction()
  {
    $src = $this->ref->val('src');
    $lambda = create_function('$recs,$ind,$rec,$name', $src->nodeValue);

    $val = call_user_func(
      $lambda,
      $this->ref->val('recs'),
      $this->ref->val('ind'),
      $this->ref->val('rec'),
      $this->ref->val('name')
    );

    return $val;
  }

  private function execUserFunction()
  {
    list($recs, $ind, $rec, $name, $func_name) = array(
      $this->ref->val('recs'),
      $this->ref->val('ind'),
      $this->ref->val('rec'),
      $this->ref->val('name'),
      $this->ref->val('func_name')
    );

    $args = $this->getCommandArgs();
    $nargs = $this->getCommandNamedArgs();

    $ind = $nargs['ind'] ? $nargs['ind'] : $ind;
    $name = $nargs['name'] ? $nargs['name'] : $name;

    $defined_funcs = get_defined_functions();

    if(in_array($func_name, $defined_funcs['internal'])){
      $val = call_user_func($func_name, $rec[$name]);
    }
    else{
      $val = call_user_func($func_name, $recs, $ind, $rec, $name, $args);
    }
    return $val;
  }

  public function execute($refVar = null){
    $src = $this->ref->val('src');
    $func_name = $this->checkFuncName($src);

    if($func_name === false){
      throw new \Exception(sprintf('Function "%s" not found', $func_name));
    }

    if(is_null($func_name)){
      $val = $this->execLambdaFunction();
    }
    else{
      $this->ref->add(array('func_name' => $func_name));
      $val = $this->execUserFunction();
    }

    return $src->getAttribute('not') ? !$val : $val;
  }
}

abstract class AbstractTableCommand extends AbstractCommandWithReference
{
  public function createCommand($ref)
  {
    $cmd_name = $ref['src']->getAttribute('name');
    preg_match('/^cmd:(.*)/', $cmd_name, $m);

    if($cmd_classname = $m[1]){
      return new $cmd_classname($ref);
    }
    return new UserFunction($ref);
  }
}

class Translate extends AbstractTableCommand
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
            (strpos($name, 'func:') !== 0 && !array_key_exists($name, $rec))
            || $rec[$name] === '_DITTO_'){
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

          $newVal = $this->createCommand($ref)->execute(new \ManipulatorRefVar());

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

class Validate extends AbstractTableCommand
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
            $this->markRecord($name, $ind, 'Missing Column: ' . $name, 'error');
          }
          if($rec[$name] === '_DITTO_'){
            continue;
          }

          $BinTest = new \GuBinTest();

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
            $chk = $this->createCommand($ref)->execute(new \ManipulatorRefVar());

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

class Filter extends AbstractTableCommand
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
          $chk = $this->createCommand($ref)->execute(new \ManipulatorRefVar());

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

class isDistinct extends AbstractCommandWithReference
{
  public function execute($refVar = null)
  {
    list($recs, $rec, $name, $ind) = array(
      $this->ref->val('recs'),
      $this->ref->val('rec'),
      $this->ref->val('name'),
      $this->ref->val('ind'),
    );

    $val = $rec[$name];

    foreach($recs as $i => $r){
      if(empty($r[$name]) || $i == $ind){
        continue;
      }
      if($r[$name] === $val){
        return false;
      }
    }
    return true;
  }
}