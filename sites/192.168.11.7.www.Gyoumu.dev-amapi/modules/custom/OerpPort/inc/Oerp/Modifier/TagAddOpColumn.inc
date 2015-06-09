<?php
namespace Oerp\Modifier;

class TagAddOpColumn extends \Gulei\Modifier\Tag\_Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
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
           '#op' => array(
             'data' => '',
             'class' => 'oerp-op',
           )
      ),
      $row['data']
    );

    $op_type = $ref->val('_param', 'oerp:op-type');

    if ($op_type == 'edit'
        || preg_match('/^C[UX][DX]$/', $op_type)
    ) {
      $srcEdit = <<<XML
<div class="oerp-button oerp-btn-add ui-state-default ui-corner-all">
  <span class="oerp-icon ui-icon ui-icon-document"></span>
</div>
XML;

      $row['data']['#op']['data'] = $srcEdit;
    }
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}