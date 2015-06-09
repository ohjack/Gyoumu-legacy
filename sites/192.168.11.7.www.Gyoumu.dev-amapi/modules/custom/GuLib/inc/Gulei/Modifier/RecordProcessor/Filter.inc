<?php
namespace Gulei\Modifier\RecordProcessor;

class Filter extends _Abstract
{
  protected function getType()
  {
    return 'filter';
  }

  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }
}