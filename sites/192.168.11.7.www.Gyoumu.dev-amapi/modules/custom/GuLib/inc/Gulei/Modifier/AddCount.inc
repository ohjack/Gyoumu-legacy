<?php
namespace Gulei\Modifier;

class AddCount extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return $ref->val('_param', 'count');
  }

  protected function modifyRows($RefVars, $ref)
  {
    $rows = &$RefVars->refVar('rows');

    $c = 1;
    foreach($rows as &$row)
    {
      if($ref->val('_param', 'copy_method') == 'ditto'
        && in_array('_DITTO_', $row, true) ) {

        $row['%count'] = '_DITTO_';
      }
      else{
        $row['%count'] = $c;
        $c++;
      }
    }
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