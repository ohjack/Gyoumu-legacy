<?php
namespace Gulei\Command;

class UserFunction extends \Gulei\Command\WithReference\WithTable
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

        if(function_exists($m[1])){
          return $m[1];
        }

        echo '<pre>';
        throw new \Exception(sprintf('Function "PREFIX_%s" not found', $m[1]));
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