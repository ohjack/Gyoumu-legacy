<?php
namespace Chte\Processor;

abstract class Abstract_PC_MB extends \Chte\Processor\Record {
  abstract protected function getPcField();
  abstract protected function getMbField();

  protected function getHtml($rec, $type) {
    $fpath = sprintf(
      '%s/Abstract_PC_MB/%s-%s.xml', __DIR__, $type, $rec['tmp_type']);

    $tpl = new \PHPTAL($fpath);
    $tpl->rec = $rec;

    foreach (array('tmp_Src1', 'tmp_Src2') as $arg) {
      $tpl->$arg = $rec[$arg];
    }
    return $tpl->execute();
  }

  protected function getPcHtml($rec) {
    $src = $this->getHtml($rec, 'pc');
    $src = html_entity_decode($src);
    return $src;
  }

  protected function getMbHtml($rec) {
    return html_entity_decode($this->getHtml($rec, 'mb'));
  }

  protected function setHTML() {
    foreach ($this->input as &$rec) {
      $rec[$this->getPcField()] = $this->getPcHtml($rec);
      $rec[$this->getMbField()] = $this->getMbHtml($rec);
    }
  }

  protected function prepareRecords() {
    $this->setHTML();
  }

  public function getRecords() {
    $this->prepareRecords();
    return parent::getRecords();
  }
}