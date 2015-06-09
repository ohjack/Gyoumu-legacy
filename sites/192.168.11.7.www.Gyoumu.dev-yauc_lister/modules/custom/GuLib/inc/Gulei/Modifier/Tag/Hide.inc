<?php
namespace Gulei\Modifier\Tag;

class Hide extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    $key = $RefVars->val('key');

    if($ref->val($key, '_param', 'hide')){
      return true;
    }
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');
    $cell['style'] = 'display:none;';
  }
}