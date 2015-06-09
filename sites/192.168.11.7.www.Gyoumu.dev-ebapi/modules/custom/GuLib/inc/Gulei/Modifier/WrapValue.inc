<?php
namespace Gulei\Modifier;

class WrapValue extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');

    $mani = new \Gulei\Manipulator\GuValue();
    $mani->wrap($cell);
  }
}