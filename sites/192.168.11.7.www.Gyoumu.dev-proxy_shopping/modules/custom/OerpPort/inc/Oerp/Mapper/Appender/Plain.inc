<?php
namespace Oerp\Mapper\Appender;

class Plain extends \oerp\Mapper\Appender\_Abstract
{
  public function isApplicable()
  {
    return true;
  }

  protected function adapt()
  {
    return $this->ref->val('val');
  }
}