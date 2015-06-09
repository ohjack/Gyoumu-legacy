<?php
namespace Oerp\Delivery\Provider\ReportPrint;

class Packing_List extends \Oerp\Delivery\Provider\ReportPrint\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  public function getThemeSrc() {
    return sprintf(
      '<div style="width: 700px;">%s</div>',
      $this->getTable()->theme()
    );
  }
}