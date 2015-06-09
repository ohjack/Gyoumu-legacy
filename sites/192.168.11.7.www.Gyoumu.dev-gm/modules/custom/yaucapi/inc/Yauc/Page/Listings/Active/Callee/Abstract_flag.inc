<?php
namespace yauc\Page\Listings\Active\Callee;

abstract class Abstract_flag extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function flagSelection($flag, $flag_val)
  {
    foreach($this->FormPage->getSelection() as $sel){
      $CtrlFlag = new \YaucFlagController($sel);
      $CtrlFlag->setFlag($flag, $flag_val);
    }
  }
}