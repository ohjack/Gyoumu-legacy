<?php
namespace Gulei\Modifier;

class Nl2br extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    $key = $RefVars->val('key');
    $row = $RefVars->val('row');

    if($row['data'][$key]['no-nl2br'] || $ref->val($key, '_param', 'no-nl2br')){
      return false;
    }

    if($ref->val($key, '_param', 'nl2br')){
      return true;
    }

    if($ref->val('_param', 'nl2br')){
      return true;
    }
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');
    $toks = explode("\n", $cell);

    if(count($toks) == 1){
      return false;
    }

    foreach($toks as &$tok){
      $tok = '<span>' . $tok . '</span>';
    }

    $newVal = '<span>' . implode('<br/>', $toks) . '</span>';
    $mani = new \Gulei\Manipulator\GuValue();
    $mani->replace($newVal, $cell);
   }
}