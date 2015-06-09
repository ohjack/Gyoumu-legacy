<?php
namespace Gulei\Modifier;

class Pre extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    if($ref->val('_param', 'pre'))
      return true;

    $key = $RefVars->val('key');

    if($ref->val($key, '_param', 'pre'))
      return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');

    $mani = new \Gulei\Manipulator\GuValue();
    $mani->wrap($cell);

    $m = $mani->match($cell);
    $cell = str_replace($m[0], "<pre>$m[0]</pre>", $cell);
  }
}