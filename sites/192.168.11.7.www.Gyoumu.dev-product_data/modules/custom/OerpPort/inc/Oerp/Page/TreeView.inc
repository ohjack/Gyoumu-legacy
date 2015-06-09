<?php
namespace Oerp\Page;

class TreeView extends \Oerp\Page\Abstract_TreeView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    list($vid, $param_over) = $this->args;
    $param = array();

    if ($arch = $param_over['arch']) {
      $Param = new \Gulei\Arch\Param($arch);
      $param_arch = $Param->getReference();

      if ($param_arch['_param']) {
        $param = $param_arch['_param'];
      }
    }

    $param = is_array($param_over) ? array_merge($param, $param_over) : $param;
    return $param;
  }

  protected function getToolbarBottom()
  {
    $calls = array(
      "Oerp\\Page\\TreeView\\Callee\\Save",
      "Oerp\\Page\\TreeView\\Callee\\Download_CSV"
    );
    return $this->getLocalCalleeEle($calls);
  }
}