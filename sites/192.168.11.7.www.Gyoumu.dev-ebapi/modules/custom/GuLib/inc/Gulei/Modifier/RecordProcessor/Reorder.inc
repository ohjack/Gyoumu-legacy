<?php
namespace Gulei\Modifier\RecordProcessor;

class Reorder extends _Abstract
{
  protected function getType()
  {
    return 'reorder';
  }

  protected function isModApplicable($RefVars, $ref)
  {
    return !$ref->val('_param', 'no_reorder');
  }
}
