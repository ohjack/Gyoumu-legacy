<?php
namespace Oerp\Modifier;

class TagAlterRows extends \Gulei\Modifier\Tag\_Abstract
{
  protected function isModApplicable($RefVars, $ref)
  {
    return true;
  }

  protected function modifyRow($RefVars, $ref)
  {
    $ind = $RefVars->val('ind');
    $rec = $ref->val('_recs', $ind);

    $row = &$RefVars->refVar('row');
    $row['data']['#op']['class'] = 'oerp-op';

    $mani = new \Gulei\Manipulator\GuValue();
    $rec['id'] = $mani->value($rec['id']);
    $row['oerp_rid'] = $rec['id'];

    $classes = explode(' ', $row['class']);

    if(!$rec['id'] && !in_array('_DITTO_', $rec, true)) {
      $classes[] = 'oerp-tpl';
    }

    $row['class'] = implode($classes, ' ');

    $op_type = $ref->val('_param', 'oerp:op-type');
    $toEdit = $toDel = $toSel = false;

    if($op_type == 'edit'){
      $toEdit = true;
      $toDel = true;
    }

    if(preg_match('/^[CX]U[DX]$/', $op_type)){
      $toEdit = true;
    }

    if(preg_match('/^[CX][UX]D$/', $op_type)){
      $toDel = true;
    }

    if($op_type == 'select-one'){
      $toSel = true;
    }

    $srcEdit = <<<XML
<div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all">
  <span class="oerp-icon ui-icon ui-icon-pencil"></span>
</div>
XML;

    $srcDel = <<<XML
<div class="oerp-button oerp-btn-del ui-state-default ui-corner-all">
  <span class="oerp-icon ui-icon ui-icon-close"></span>
</div>
XML;

    $srcSel = <<<XML
<div class="oerp-button oerp-btn-select ui-state-default ui-corner-all">
  <span class="oerp-icon ui-icon ui-icon-check"></span>
</div>
XML;

    $row['data']['#op']['data'] = sprintf('<div>%s%s%s</div>',
      $toEdit ? $srcEdit : '', $toDel ? $srcDel : '', $toSel ? $srcSel : ''
    );

    $row['data']['#op']['no-nl2br'] = 1;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    $key = $RefVars->val('key');
    return !!$ref->val('_param', 'fields', $key);
  }

  protected function modify($RefVars, $ref)
  {
    $key = $RefVars->val('key');
    $cell = &$RefVars->refVar('cell');
    $cell['oerp_field'] = $key;

    if($relation = $ref->val('_param', 'fields', $key, 'relation'))
      $cell['oerp_relation'] = $relation;
  }
}