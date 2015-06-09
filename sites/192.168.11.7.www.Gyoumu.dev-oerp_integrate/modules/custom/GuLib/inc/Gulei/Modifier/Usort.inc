<?php
namespace Gulei\Modifier;

class Usort extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return $ref->val('_param', 'sort');
  }

  protected function modifyRows($RefVars, $ref)
  {
    $rows = &$RefVars->refVar('rows');
    usort($rows, $ref->val('_param', 'sort'));
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}