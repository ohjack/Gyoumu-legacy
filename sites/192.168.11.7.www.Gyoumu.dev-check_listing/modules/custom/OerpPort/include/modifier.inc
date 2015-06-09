<?php
use Gulei as G;
module_load_include('inc', 'GuLib', '/include/processor');

class OerpTagModAddOpColumn extends G\AbstractTagModifier
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

class OerpTagModUseParamString extends G\AbstractTagModifier
{
  protected function isModApplicable($RefVars, $ref)
  {
    return !!$ref->val('_param', 'fields');
  }

  protected function isCellApplicable($RefVars, $ref)
  {
    $key = $RefVars->val('key');
    return strlen($ref->val('_param', 'fields', $key, 'string'));
  }

  protected function modify($RefVars, $ref)
  {
    $key = $RefVars->val('key');
    $newString = $ref->val('_param', 'fields', $key, 'string');

    $cell = &$RefVars->refVar('cell');
    $mani = new ManipulatorGuValue();
    $mani->replace($newString, $cell['data']);
  }
}

class OerpTagModMergeOp extends G\AbstractTagModifier
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

class OerpTagModAlterRows extends G\AbstractTagModifier
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

    $mani = new \ManipulatorGuValue();
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

class OerpPackHeaderRawMods extends G\AbstractModPack
{
  protected function init()
  {
    $pack = new G\PackHeaderRawMods();
    $this->append($pack->getItems());
    $this->append(
      array(
           new OerpTagModAddOpColumn(),
           new OerpTagModUseParamString(),
      )
    );
  }
}

class OerpPackRowRawMods extends G\AbstractModPack
{
  protected function init()
  {
    $pack = new G\PackRowRawMods();
    $this->append($pack->getItems());
    $this->append(
      array(
        new OerpTagModAlterRows(),
        new OerpTagModMergeOp(),
      )
    );
  }
}