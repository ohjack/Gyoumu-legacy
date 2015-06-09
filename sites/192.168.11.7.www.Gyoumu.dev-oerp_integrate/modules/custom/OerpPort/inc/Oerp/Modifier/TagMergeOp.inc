<?php
namespace Oerp\Modifier;

class TagMergeOp extends \Gulei\Modifier\Tag\_Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function modifyRows($RefVars, $ref)
  {
    $rows = &$RefVars->refVar('rows');

    foreach ($rows as $ind => $row) {
      if ($row['merged']) {
        unset($rows[$ind]['data']['#op']);

        for ($c = 1; $ind - $c >= 0; $c++) {
          if (isset($rows[$ind - $c]['data']['#op'])) {
            $rows[$ind - $c]['data']['#op']['rowspan']++;
            break;
          }
        }
      }
    }
  }

  protected function isRowApplicable($RefVars, $ref)
  {
    return false;
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