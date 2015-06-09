<?php
namespace Oerp\Page;

class FormView extends \Oerp\Page\Abstract_FormView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    list($model, $vid, $rid, $param_base) = $this->args;

    $param = array(
      'model' => $model,
      'vid' => $vid,
      'rid' => $rid,
    );

    $param = is_array($param_base) ? array_merge($param_base, $param) : $param;
    return $param;
  }
}