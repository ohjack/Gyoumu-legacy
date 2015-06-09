<?php
namespace Gulei\Modifier;

class CopyDuplicate extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    $method = $ref->val('_param', 'copy_method');
    if($method == 'duplicate'){
      return true;
    }
  }

  protected function isCellApplicable($RefVals, $ref)
  {
    return true;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $rows_send = array();
    $rows = &$RefVars->refVar('rows');
    $names = array_keys($rows[0]);

    foreach($rows as $ind => $row){
      $row_send = array();

      foreach ($names as $name) {
        if (!isset($row[$name])) {
          for ($i = $ind; $i >= 0; $i--) {
            if (isset($rows[$i][$name])) {
              $fld = $rows[$i][$name];
              break;
            }
          }
        }
        else {
          $fld = $row[$name];
        }
        $row_send[$name] = $fld;
      }
      $rows_send[] = $row_send;
    }
    $rows = $rows_send;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}
