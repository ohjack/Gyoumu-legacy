<?php
namespace Gulei\Form\Element;

abstract class Abstract_tWithOption extends _Abstract
{
  protected function getParamDefault()
  {
    $paramDefault = parent::getParamDefault();

    if($paramDefault){
      return (array)json_decode($paramDefault);
    }
    else{
      return null;
    }
  }

  protected function getEntryTemplate()
  {
    $tpl = parent::getEntryTemplate();
    $opts = array();

    foreach ($this->xp('./option') as $ndOpt) {
      $attOpt = $ndOpt->attributes;
      $opts[$attOpt->getNamedItem('value')->value] = $ndOpt->nodeValue;
    }

    $tpl['#options'] = $opts;

    if ($tpl['#attributes']['readonly']) {
      $tpl['#attributes']['disabled'] = 'disabled';
      unset($tpl['#attributes']['readonly']);
    }
    return $tpl;
  }
}