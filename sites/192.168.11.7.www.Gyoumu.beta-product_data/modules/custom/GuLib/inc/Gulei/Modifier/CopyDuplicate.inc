<?php
namespace Gulei\Modifier;

class CopyDuplicate extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    if($ref->val('_param', 'copy_method') == 'duplicate')
      return true;
  }

  protected function isCellApplicable($RefVals, $ref)
  {
    return true;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $rows_send = array();
    $rows = &$RefVars->refVar('rows');

    foreach($rows as $row){
      $row_send = array();

      foreach($row as $name => $fld){
        if($fld == '_DITTO_'){
          $fld = $rows_send[count($rows_send) - 1][$name];
        }
        $row_send[$name] = $fld;
      }
      $rows_send[] = $row_send;
    }
    $rows = $rows_send;
//    dpr($rows);die;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}
