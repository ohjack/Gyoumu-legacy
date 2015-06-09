<?php
namespace Gulei\Modifier;

class EscapeXml extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return $ref->val('_param', 'escape');
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');

    $mani = new \Gulei\Manipulator\GuValue();
    $newVal = htmlspecialchars($mani->value($cell));

    $mani->replace($newVal, $cell);
  }
}