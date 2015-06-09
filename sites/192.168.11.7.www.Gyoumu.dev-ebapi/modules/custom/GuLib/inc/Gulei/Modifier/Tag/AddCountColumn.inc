<?php
namespace Gulei\Modifier\Tag;

class AddCountColumn extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    if($ref->val('_param', 'count'))
      return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modifyRow($RefVars, $ref)
  {
    $row = &$RefVars->refVar('row');
    $row['data'] = array_merge(
      array(
           '%count' => array(
             'data' => '#',
             'gu_name' => '%count',
             'rowspan' => 1,
           ),
      ),
      $row['data']
    );
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}