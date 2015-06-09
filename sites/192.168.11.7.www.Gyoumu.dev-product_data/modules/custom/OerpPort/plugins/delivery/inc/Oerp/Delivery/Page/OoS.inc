<?php
namespace Oerp\Delivery\Page;

class OoS extends \Oerp\Page\Abstract_TreeView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function getArch() {
    return file_get_contents(__DIR__ . '/OoS.xml');
  }

  protected function getParam()
  {
    $param = array();
    $param['arch'] = $this->getArch();
    $param['criteria'][] = array('state', '=', 'confirmed');
    $param['criteria'][] = array('x_seller', '<>', FALSE);
    $param['oerp:op-type'] = 'select-many';
    $param['search-custom'] = array(
      'date_done', 'date', 'x_seller', 'origin', 'carrier_id'
    );
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