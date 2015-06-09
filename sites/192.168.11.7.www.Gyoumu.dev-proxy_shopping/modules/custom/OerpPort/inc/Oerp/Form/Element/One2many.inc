<?php
namespace Oerp\Form\Element;

class One2many extends _Abstract
{
  public function prepareSend()
  {
    return $this->args['nested_value'];
  }

  protected function getPrepareSendJs()
  {
    return <<<JS
return {
  '#name' : fld['name'],
  '#type' : 'one2many',
  '#value' : fld['value']
};
JS;

  }

  protected function getEntryTemplate()
  {
    $args = $this->args;
    $fld = $args['field'];

//        get ids of @fld_name for this record
    $q = new \Oerp\Query\Basic(
      $args['model'],
      null,
      array($args['name']),
      array($args['rid'])
    );

    $resp = $q->raw();
    $ids = $resp[0][$args['name']];

    if (empty($ids)) {
      $ids = array(0);
    }

//        build element 'oerp_treeview'
    $ele['#type'] = 'oerp_treeview';

    $param = array(
      'oerp:model' => $fld['relation'],
      'oerp:ids' => $ids,
      'name' => $args['name'],
      'arch' => $fld['views']['tree']['arch'],
      'fields' => $fld['views']['tree']['fields'],
      'caption' => $this->getCaption(),
      'key' => sprintf('%s-%s', $args['key'], str_replace('_', '-', $args['name'])),
      'oerp:op-type' => 'edit',
      'oerp:type' => 'one2many',
      'escape' => true,
    );

    if (!$param['arch']) {
      $Arch = new \Gulei\Arch\Xml($args['arch']);
      $Fld = $Arch->xp('./tree[1]')->item(0);
      $tree_arch = $Arch->dump($Fld);

      $param['arch'] = $tree_arch;
      unset($param['fields']);
    }

    if ($table_param = $fld['table_param']) {
      $param = array_merge($param, $table_param);
    }

    if (isset($fld['views']['form'])) {
      $cid = sprintf('formview:%s-%s', $args['vname'], $param['name']);
      cache_set($cid, $fld['views']['form'], 'cache_oerpport', CACHE_TEMPORARY);
      $param['cache_id'] = $cid;
    }

    $ele['#param'] = $param;

//        HTML snippet anchor
    $ele['#prefix'] =
        '<div id="edit-' . $param['key'] . '-wrapper">';

    $ele['#suffix'] = '</div>';
    return $ele;
  }
}