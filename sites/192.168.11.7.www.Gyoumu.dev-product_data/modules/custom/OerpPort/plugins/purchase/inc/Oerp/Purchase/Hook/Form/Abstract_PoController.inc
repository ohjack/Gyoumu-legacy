<?php
namespace Oerp\Purchase\Hook\Form;

abstract class Abstract_PoController {
  protected $param;
  protected $rec;
  protected $state;

  abstract protected function _exe($param);
  abstract protected function isApplicableState();

  protected function init($param) {
    $this->param = $param;
    $this->rec = $this->getRec();
    $this->state = $this->rec['state'];
  }

  protected function getRec() {
    $prov = new \Oerp\Purchase\Provider\PoLines($this->param['rid']);
    $recs = $prov->getData();
    return $recs[0];
  }

  protected function getPicking() {
    $oline_ids = array();
    foreach ($this->rec['order_line'] as $line) {
      $oline_ids[] = $line['id'];
    }

    $provPo = new \Oerp\Purchase\Provider\PoPicking(
      array($this->param['rid']));

    $picking = $provPo->getData();
    return array_shift($picking);
  }

  protected function getSearchType() {
    switch($this->rec['state']) {
      case 'draft':
      case 'confirmed':
        return 'confirmed';

      case 'approved':
      case 'done':
        return 'assigned';
    }
    return FALSE;
  }

  public function execute($param) {
    $this->init($param);

    if (!$this->isApplicableState()) {
      return FALSE;
    }
    return $this->_exe($param);
  }

  protected function getQuery() {
    return new \Oerp\Query('purchase.order', NULL, NULL, $this->param['rid']);
  }
}