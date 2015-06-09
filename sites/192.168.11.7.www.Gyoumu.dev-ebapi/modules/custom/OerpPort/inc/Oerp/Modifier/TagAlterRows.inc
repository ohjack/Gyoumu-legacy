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

    /* TODO using ind to select rec not always correct e.g dittos included */
    $rec = $ref->val('_recs', $ind);
    $row = &$RefVars->refVar('row');

    $mani = new \Gulei\Manipulator\GuValue();
    $rec['id'] = $mani->value($rec['id']);

    if (($id = $row['data']['id']['data'])) {
      $row['oerp_rid'] = $id;
    }
    else {
      $row['oerp_rid'] = $rec['id'];
    }
    $classes = explode(' ', $row['class']);

    if (!$row['oerp_rid'] && !$row['merged']) {
      $classes[] = 'oerp-tpl hide';
    }

    $row['class'] = implode($classes, ' ');

    $op_type = $ref->val('_param', 'oerp:op-type');

    if ($op_type === FALSE) {
      return;
    }

    $row['data']['#op']['class'] = 'oerp-op';
    $sig = $ref->val('_param', 'signature');
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

    if($op_type == 'select-one' || $op_type == 'select-many'){
      $toSel = true;
    }

    $srcEdit = <<<XML
<div class="oerp-button oerp-btn-edit ui-state-default ui-corner-all %s" signature="$sig">
  <span class="oerp-icon ui-icon ui-icon-pencil"></span>
</div>
XML;

    $srcEdit = sprintf($srcEdit, $toEdit ? '' : 'hide');

    $srcDel = <<<XML
<div class="oerp-button oerp-btn-del ui-state-default ui-corner-all %s" signature="$sig">
  <span class="oerp-icon ui-icon ui-icon-close"></span>
</div>
XML;

    $srcDel = sprintf($srcDel, $toDel ? '' : 'hide');

    $srcSel = <<<XML
<div class="oerp-button oerp-btn-select ui-state-default ui-corner-all %s" signature="$sig">
  <span class="oerp-icon ui-icon ui-icon-check"></span>
</div>
XML;

    $srcSel = sprintf($srcSel, $toSel ? '' : 'hide');
    $srcEx = $ref->val('_param', 'ex_ops');

    $row['data']['#op']['data'] = sprintf(
      '<div>%s%s%s%s</div>', $srcEdit, $srcDel, $srcSel, $srcEx);

    $row['data']['#op']['no-nl2br'] = 1;
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    return true;
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