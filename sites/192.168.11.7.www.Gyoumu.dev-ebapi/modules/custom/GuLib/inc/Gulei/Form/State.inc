<?php
namespace Gulei\Form;

class State {
  protected $state;

  public function __construct($form_state) {
    $this->state = $form_state;
  }

  public function getVals() {
    return $this->state['values'];
  }

  public function getVal($name) {
    return $this->state['values'][$name];
  }

  public function getClickedButton() {
    return $this->state['clicked_button'];
  }
}