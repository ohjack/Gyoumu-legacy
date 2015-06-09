<?php
namespace Oerp\Purchase\Page\Orders\Callee;

class Merge extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    echo 'in';die;
  }
}