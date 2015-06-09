<?php
namespace Gulei\Modifier\RecordProcessor;

class Validate extends _Abstract
{
  protected function getType()
  {
    return 'validate';
  }

  protected function isModApplicable($RefVars, $ref)
  {
    return !$ref->val('_param', 'no_validate');
  }
}
