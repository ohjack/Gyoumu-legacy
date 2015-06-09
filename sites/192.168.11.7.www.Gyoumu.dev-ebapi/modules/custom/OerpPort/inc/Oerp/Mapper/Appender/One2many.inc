<?php
namespace Oerp\Mapper\Appender;

class One2many extends \Oerp\Mapper\Appender\_Abstract
{
  public function isApplicable()
  {
    $fld = $this->ref->val('field');
    return $fld->getAttribute('ttype') == 'one2many';
  }

  protected function adapt()
  {
    return array(array(0, 0, $this->ref->val('val')));
  }
}