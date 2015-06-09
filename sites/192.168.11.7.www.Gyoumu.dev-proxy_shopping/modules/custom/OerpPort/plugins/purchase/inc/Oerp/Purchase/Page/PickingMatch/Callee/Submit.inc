<?php
namespace Oerp\Purchase\Page\PickingMatch\Callee;

class Submit extends \Gulei\Page\Form\Callee\_Abstract {
  protected $send_ref;
  protected $vals;
  protected $po;

  protected function getClassname() {
    return __CLASS__;
  }

  protected function init($form_state) {
    $StateCtrl = new \Gulei\Form\State($form_state);
    $this->vals = $StateCtrl->getVals();

    $send_ref = (array)json_decode($this->vals['send']);
    $send_ref = (array)$send_ref['vals'];
    $this->send_ref = array();

    foreach ($send_ref as $key => $val) {
      preg_match('/.*\[(.*)\]\[(.*)\]/', $key, $m);
      $this->send_ref[$m[1]][$m[2]] = $val;
    }

    $this->po = $this->FormPage->getPo();
  }

  public function execute($form, &$form_state) {
    $this->init($form_state);

    if (!$ref_lines = $this->send_ref) {
      return FALSE;
    }

    $Ctrl = new \Oerp\Purchase\Controller\PoWithMove($this->po);
    $Ctrl->createPoMoves($ref_lines);
    $Ctrl->postfix();
    $this->reload();
  }
}