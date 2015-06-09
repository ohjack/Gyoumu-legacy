<?php
namespace Ebapi\Form\Widget;

abstract class _Abstract
{
  protected $form_state;

  abstract public function getFormEntry();

  public function __construct($form_state)
  {
    $this->form_state = $form_state;
  }

  protected function getValue($name = null)
  {
    if(is_null($name)){
      return $this->form_state['values'];
    }
    return $this->form_state['values'][$name];
  }
}