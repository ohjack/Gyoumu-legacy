<?php
namespace Gulei;
require_once 'themer.inc';

class Pager implements InterfaceThemeable
{
  private $param;

  public function __construct($param)
  {
    $this->param = new \stdClass();
    $this->param->items = $param['items_per_page'];
    $this->param->page = $_GET['page'] ? $_GET['page'] : 0;
    $this->param->total = ceil($param['count'] / $param['items_per_page']);
    $this->param->from = $this->param->page * $this->param->items;
  }

  protected function isSetup()
  {
    return $this->param->items;
  }

  public function getParam()
  {
    return $this->param;
  }

  public function theme()
  {
    if(!$this->isSetup())
      return '';

    $GLOBALS['pager_page_array'] = array($this->param->page);
    $GLOBALS['pager_total'] = array($this->param->total);
    return theme_pager(array(), $this->param->items);
  }
}