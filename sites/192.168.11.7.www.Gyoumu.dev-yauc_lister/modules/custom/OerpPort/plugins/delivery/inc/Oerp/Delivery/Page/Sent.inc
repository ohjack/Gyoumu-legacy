<?php
namespace Oerp\Delivery\Page;

class Sent extends \Oerp\Page\Abstract_TreeView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    $param = array();
    $param['arch'] = file_get_contents(__DIR__ . '/Sent.xml');
    $param['criteria'][] = array('state', '=', 'done');

    if(!$_GET['cri']){
      $pat = 'Y-m-d H:i:s';
      $param['criteria'][] = array('date_done', '>=', date($pat, mktime(0, 0, 0)));
      $param['criteria'][] = array('date_done', '<=', date($pat, mktime(23, 59, 59)));
    }

    $param['oerp:op-type'] = '';
    $param['search-custom'] = array(
      'date_done', 'date', 'x_seller', 'origin', 'x_tnum'
    );
    return $param;
  }

  protected function getToolbarBottom()
  {
    $calls = array(
      "Oerp\\Delivery\\Page\\Callee\\Download_Querying",
      "Oerp\\Delivery\\Page\\Callee\\Download_Sent\\No__track",
      "Oerp\\Delivery\\Page\\Callee\\Download_Sent\\Japan",
    );
    return $this->getLocalCalleeEle($calls);
  }
}