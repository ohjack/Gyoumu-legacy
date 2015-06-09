<?php
namespace Gulei\Page\Themer;

class Pane extends \Gulei\Page\Themer\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function theme(&$ele)
  {
    return sprintf(
      '<div class="pane"><div class="caption">%s</div><div class="pane-content">%s</div></div>',
      $ele['#name'],
      gulib_render($ele, $this->FormPage)
    );
  }
}