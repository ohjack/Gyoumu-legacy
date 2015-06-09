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
    $op_type = $ref->val('_param', 'oerp:op-type');

    if ($op_type === FALSE) {
      return;
    }

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

    $sig = $ref->val('_param', 'signature');
    $src = '';

    if ($op_type == 'edit'
        || preg_match('/^C[UX][DX]$/', $op_type)
    ) {
      $src .= <<<XML
<div class="oerp-button oerp-btn-add ui-state-default ui-corner-all" signature="$sig">
  <span class="oerp-icon ui-icon ui-icon-document"></span>
</div>
XML;
    }

    if($op_type === 'select-many'){
      $src .= <<<XML
<div class="oerp-button oerp-btn-select ui-state-default ui-corner-all" signature="$sig">
  <span class="oerp-icon ui-icon ui-icon-check"></span>
</div>
XML;
    }

    $row['data']['#op']['data'] = $src;
  }

  protected function modify($RefVars, $ref)
  {
    return null;
  }
}