<?php
namespace Gulei\Modifier;

class Mapping extends \Gulei\Modifier\_Abstract
{
  protected function isModApplicable($RefVar, $ref)
  {
    return true;
  }

  protected function createMapper($rows)
  {
    return new \Gulei\Mapper\Basic($rows);
  }

  protected function modifyRows($RefVar, $ref)
  {
    $rows = &$RefVar->refVar('rows');
    $rows = $this->createMapper($rows)->exe($ref->val('_arch'));
  }

  protected function isCellApplicable($RefVar, $ref)
  {
    return false;
  }

  protected function modify($RefVar, $ref)
  {
    return null;
  }
}