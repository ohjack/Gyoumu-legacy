<?php
namespace Gulei\Modifier\Tag;

class CopyDitto extends _Abstract
{
  function __construct($keys = null)
  {
    parent::__construct('data', 'data', $keys);
  }

  protected function isModApplicable($RefVars, $ref)
  {
    if($ref->val('_param', 'copy_method') == 'ditto')
      return true;
  }

  protected function isCellApplicable($RefVals, $ref)
  {
    return true;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $cell_bg = array('merge-even', 'merge-odd');
    $row_num = 0;

    $rows = &$RefVars->refVar('rows');

    foreach($rows as $ind => $row){
      $hasDitto = false;

      foreach($row['data'] as $key => $val){
        if($val['data'] === '_DITTO_'){
          $hasDitto = true;

          for($c = 1; $c <= $ind; $c++){
            if(isset($rows[$ind - $c]['data'][$key]['rowspan'])){
              $rows[$ind - $c]['data'][$key]['rowspan']++;
              break;
            }
          }
          unset($rows[$ind]['data'][$key]);
        }
      }

      if($hasDitto){
        $rows[$ind]['merged'] = '1';
      }
      else{
        $row_num++;
      }

      $merge_class = $cell_bg[$row_num % 2];

      if(isset($rows[$ind]['class'])){
        $classes = explode(' ', $rows[$ind]['class']);
        $classes[] = $merge_class;
        $rows[$ind]['class'] = implode($classes, ' ');
      }
      else{
        $rows[$ind]['class'] = $merge_class;
      }
    }
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}
