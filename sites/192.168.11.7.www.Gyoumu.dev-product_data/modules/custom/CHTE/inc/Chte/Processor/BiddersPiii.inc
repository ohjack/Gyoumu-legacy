<?php
namespace Chte\Processor;

class BiddersPiii extends \Chte\Processor\Abstract_PC_MB {
  protected function getPcField() {
    return 'Caption';
  }

  protected function getMbField() {
    return 'KtaiCaption';
  }

  protected function setHTML() {
    parent::setHTML();

    foreach ($this->input as &$rec) {
      $comm = $this->getHtml($rec, 'KtaiDetailComment');
      $comm = html_entity_decode($comm);
      $rec['KtaiDetailComment'] = $comm;
    }
  }
}