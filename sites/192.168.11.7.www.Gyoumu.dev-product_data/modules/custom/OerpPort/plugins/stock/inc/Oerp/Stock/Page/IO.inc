<?php
namespace Oerp\Stock\Page;

class IO extends \Oerp\Page\TreeView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    $arch = file_get_contents(__DIR__ . '/IO.xml');
    $param = array(
      'arch' => $arch
    );
    return $param;
  }
}