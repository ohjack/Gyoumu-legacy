<?php
namespace Oerp\Delivery\Page;

class Cancel extends \Oerp\Page\Abstract_TreeView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    $param = array();
    $param['arch'] = file_get_contents(__DIR__ . '/common.xml');
    $param['criteria'][] = array('state', '=', 'cancel');
    $param['oerp:op-type'] = '';
    $param['search-custom'] = array();
    return $param;
  }

  protected function getToolbarBottom()
  {
    return array();
  }
}