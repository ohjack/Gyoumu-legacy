<?php
namespace Gulei\Pager;

class Basic implements \Gulei\Themer\_Interface
{
  private $param;

  public function __construct($param)
  {
    $this->param = self::getPagerParam($param);
  }

  protected function isSetup()
  {
    return $this->param->items;
  }

  static public function getPagerParam($param)
  {
    $return = new \stdClass();
    $return->items = $param['items_per_page'];
    $return->page = $_GET['page'] ? $_GET['page'] : 0;
    $return->total = ceil($param['count'] / $param['items_per_page']);
    $return->from = $return->page * $return->items;
    $return->hide = $param['pager_hide'] ? true : false;
    return $return;
  }

  public function getParam()
  {
    return $this->param;
  }

  public function theme()
  {
    if(!$this->isSetup() || $this->param->hide){
      return '';
    }

    $GLOBALS['pager_page_array'] = array($this->param->page);
    $GLOBALS['pager_total'] = array($this->param->total);
    return theme_pager(array(), $this->param->items);
  }
}