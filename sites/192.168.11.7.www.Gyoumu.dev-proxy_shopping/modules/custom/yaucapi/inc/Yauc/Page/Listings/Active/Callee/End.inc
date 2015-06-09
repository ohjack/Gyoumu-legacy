<?php
namespace Yauc\Page\Listings\Active\Callee;

class End extends \Yauc\Page\Listings\Active\Callee\Abstract_flag
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $this->flagSelection('to_end', true);
    $this->flagSelection('to_kill', false);
    $this->FormPage->goBack(false);
  }
}