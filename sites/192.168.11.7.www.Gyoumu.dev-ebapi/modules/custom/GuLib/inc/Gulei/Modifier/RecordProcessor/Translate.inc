<?php
namespace Gulei\Modifier\RecordProcessor;

class Translate extends _Abstract
{
  protected function getType()
  {
    return 'translate';
  }

  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }
}