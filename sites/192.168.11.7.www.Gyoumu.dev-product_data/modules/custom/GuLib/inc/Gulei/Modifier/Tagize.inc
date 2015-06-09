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

    if ($alt_name = $ref->val($key, '_attr', 'alt_name')) {
      $cell['gu_alt_name'] = $alt_name;
    }

    if ($class = $ref->val($key, '_attr', 'class')) {
      $cell['class'] = $class;
    }

    $attrs = $ref->val($key, '_attr');

    if (is_array($attrs)) {
      foreach ($attrs as $attr => $val) {
        if (strpos($attr, 'data-') === 0) {
          $cell[$attr] = $val;
        }
      }
    }
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
