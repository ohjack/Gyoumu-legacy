<?php
namespace Oerp\Delivery\Page;

class OoS extends \Oerp\Page\Abstract_TreeView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    $param = array();
    $param['arch'] = file_get_contents(__DIR__ . '/common.xml');
    $param['criteria'][] = array('state', '=', 'confirmed');
    $param['oerp:op-type'] = 'select-many';
    $param['search-custom'] = array();
    return $param;
  }

  protected function getToolbarBottom()
  {
    $calls = array(
      "Oerp\\Delivery\\Page\\Callee\\Mark_as_READY",
      "Oerp\\Delivery\\Page\\Callee\\Mark_as_CANCEL",
      "Oerp\\Delivery\\Page\\Callee\\Download_OoS\\Querying",
    );
    return $this->getLocalCalleeEle($calls);
  }
}