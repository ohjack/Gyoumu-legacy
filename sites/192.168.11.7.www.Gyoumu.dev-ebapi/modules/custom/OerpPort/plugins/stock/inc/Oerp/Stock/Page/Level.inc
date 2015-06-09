<?php
namespace Oerp\Stock\Page;

class Level extends \Oerp\Page\Abstract_TreeView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    $arch = file_get_contents(__DIR__ . '/Level.xml');
    $param = array(
      'arch' => $arch
    );

    if($_GET['full']){
      $param['pager'] = false;
    }
    return $param;
  }

  protected function getToolbarBottom()
  {
    return array();
  }
}