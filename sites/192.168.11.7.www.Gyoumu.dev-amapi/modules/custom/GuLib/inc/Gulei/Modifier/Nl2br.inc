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

    if($ref->val($key, '_param', 'no-nl2br'))
      return false;

    if($ref->val($key, '_param', 'nl2br'))
      return true;

    if($ref->val('_param', 'nl2br'))
      return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');

    $mani = new \Gulei\Manipulator\GuValue();
    $newVal = nl2br($mani->value($cell), true);

    $mani->replace($newVal, $cell);
   }
}