<?php
namespace Oerp\Form\Element;

class PoWithHints extends \Oerp\Form\Element\One2many {
  protected function getEntryTemplate() {
    $arch = file_get_contents(__DIR__ . '/PoWithHints.xml');
    $this->args['field']['views']['tree']['arch'] = $arch;
    return parent::getEntryTemplate();
  }
}