<?php
namespace Gulei\Modifier;

class Tagize extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function modify($RefVars, $ref)
  {
    $cell = &$RefVars->refVar('cell');
    $key = $RefVars->val('key');

    $gu_name = $ref->val($key, '_attr', 'name');
    $gu_name = is_null($gu_name) ? $key : $gu_name;

    $cell = array(
      'data' => $cell,
      'gu_name' => $gu_name,
      'rowspan' => 1,
    );
  }

  protected function isRowApplicable($RefVars, $ref)
  {
    $row = $RefVars->val('row');
    return empty($row) ? false : true;
  }

  protected function modifyRow($RefVars, $ref)
  {
    $row = &$RefVars->refVar('row');
    $row = empty($row) ? array() : array('data' => $row);
  }

  protected function &getRowIterator(&$row)
  {
    return $row['data'];
  }
}
