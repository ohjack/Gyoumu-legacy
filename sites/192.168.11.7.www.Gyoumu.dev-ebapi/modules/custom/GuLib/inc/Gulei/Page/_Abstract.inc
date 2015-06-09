<?php
namespace Gulei\Page;

abstract class _Abstract
{
  protected $args;

  public function __construct($args)
  {
    $this->args = $args;
  }

  protected function redirect($form_state = null)
  {
    if($dir = $form_state['storage']['redirect']){
      header('Location: ' . $dir);
      return true;
    }
    return false;
  }
}