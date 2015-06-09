<?php
namespace Oerp\Command;

class LinkToFormView extends \Gulei\Command\WithReference\WithTable {
  public function execute($refVar = null) {
    static $model = NULL;

    if (is_null($model)) {
      $Tb = $this->ref->val('ObjTable');
      $param = $Tb->getParam();
      $model = $param['oerp:model'];
    }

    list($rec, $name) = $this->ref->vals('rec', 'name');
    $path = sprintf('oerp/view/form/%s/0/%s', $model, $rec['id']);
    return l($rec['name'], $path);
  }
}