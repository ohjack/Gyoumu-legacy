<?php
namespace Chte\Processor;

class Bidders extends \Chte\Processor\Abstract_PC_MB {
  protected function getPcField() {
    return 'Caption';
  }

  protected function getMbField() {
    return 'KtaiCaption';
  }

  protected function getPcHtml($rec) {
    return $this->getMbHtml($rec);
  }
}