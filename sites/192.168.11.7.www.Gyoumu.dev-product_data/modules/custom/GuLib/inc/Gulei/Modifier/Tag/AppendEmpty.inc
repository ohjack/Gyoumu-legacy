<?php
namespace Gulei\Modifier\Tag;

class AppendEmpty extends _Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return false;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }

  protected function isRowApplicable($RefVars, $ref)
  {
    $row = $RefVars->val('row');
    if(empty($row))
      return true;
  }

  protected function modifyRow($RefVars, $ref)
  {
    $row = &$RefVars->refVar('row');
    $row['empty'] = 1;
    $row['data'] = array();

    foreach($ref->val() as $name => $hd){
      if(strpos($name, '_') !== 0){
        if(!isset($hd['_param']['hide']) || !$hd['_param']['hide']){
          $row['data'][] = '';
        }
      }
    }
  }
}