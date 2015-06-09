<?php
namespace Oerp\Modifier;

class PrepareSend extends \Gulei\Modifier\_Abstract
{
  protected function isModApplicable($RefVar, $ref)
  {
    return true;
  }

  protected function modifyRows($RefVar, $ref)
  {
    $Cmd = new \Oerp\Modifier\Command\prepareSend($ref);
    $Cmd->execute($RefVar);
  }

  protected function isRowApplicable($RefVar, $ref)
  {
    return false;
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