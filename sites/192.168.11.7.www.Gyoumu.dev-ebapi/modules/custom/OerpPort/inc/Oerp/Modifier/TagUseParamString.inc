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

  protected function modify($RefVars, $ref) {
    $key = $RefVars->val('key');
    $new_string = '';

    if ($Arch = $ref->val("Gulei\\Provider\\Arch")) {
      $fld = $Arch->xp('//field[@name="' . $key . '"]')->item(0);
      $new_string = $fld->getAttribute('string');
    }

    if (!$new_string) {
      $new_string = $ref->val('_param', 'fields', $key, 'string');
    }

    $cell = &$RefVars->refVar('cell');
    $mani = new \Gulei\Manipulator\GuValue();
    $mani->replace($new_string, $cell['data']);
  }
}