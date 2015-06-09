<?php
namespace Gulei\Modifier;

class VisualizeBoolean extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    $cell = $RefVars->val('cell');
    return is_bool($cell);
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');
    $cell = $cell ? 'Yes' : 'No';
  }
}