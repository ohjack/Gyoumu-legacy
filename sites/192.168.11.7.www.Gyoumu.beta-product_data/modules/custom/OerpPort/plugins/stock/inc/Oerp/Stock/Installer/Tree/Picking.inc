<?php
namespace Oerp\Stock\Installer\Tree;

class Picking extends \Oerp\Installer\View\Abstract_Tree
{
  protected function getFilepath()
  {
    return __FILE__;
  }

  protected function getModel()
  {
    return 'stock.picking';
  }
}