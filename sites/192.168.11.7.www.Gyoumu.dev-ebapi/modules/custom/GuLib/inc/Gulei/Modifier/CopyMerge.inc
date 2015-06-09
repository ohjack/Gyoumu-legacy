<?php
namespace Gulei\Modifier;

class CopyMerge extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    if($ref->val('_param', 'copy_method') == 'merge')
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
      if(in_array('_DITTO_', $row, true)){
        foreach($row as $name => $fld){
          if($fld !== '_DITTO_'){
            $rows_send[count($rows_send) - 1][$name] .= "\n" . $row[$name];
          }
        }
      }
      else{
        $rows_send[] = $row;
      }
    }
    $rows = $rows_send;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}
