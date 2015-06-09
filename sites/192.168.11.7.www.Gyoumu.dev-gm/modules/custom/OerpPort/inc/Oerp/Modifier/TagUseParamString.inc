<?php
namespace Oerp\Modifier;

class TagUseParamString extends \Gulei\Modifier\Tag\_Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return !!$ref->val('_param', 'fields');
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    $key = $RefVars->val('key');
    return strlen($ref->val('_param', 'fields', $key, 'string'));
  }

  protected function modify($RefVars, $ref)
  {
    $key = $RefVars->val('key');
    $newString = $ref->val('_param', 'fields', $key, 'string');

    $cell = &$RefVars->refVar('cell');
    $mani = new \Gulei\Manipulator\GuValue();
    $mani->replace($newString, $cell['data']);
  }
}