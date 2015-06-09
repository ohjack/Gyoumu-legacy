<?php
namespace Gulei\Page\Themer;

class Panes extends \Gulei\Page\Themer\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function theme(&$ele)
  {
    return sprintf('<div class="panes">%s</div><div class="clearfix"></div>', gulib_render($ele, $this->FormPage));
  }
}