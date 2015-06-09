<?php
namespace Oerp\Gallery\Page\PhotoSubmit\Callee;

class Submit extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $h = new \Oerp\Gallery\Helper();
    $h->submitPhoto();
  }
}