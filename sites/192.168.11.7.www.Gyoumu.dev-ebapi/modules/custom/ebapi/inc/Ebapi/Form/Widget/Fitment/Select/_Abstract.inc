<?php
namespace Ebapi\Form\Widget\Fitment\Select;

abstract class _Abstract extends \Ebapi\Form\Widget\Fitment\_Abstract
{
  abstract static protected function getName();
  abstract protected function getTitle();
  abstract protected function isInYearRange();
  abstract protected function isYearDep();
  abstract protected function isMultipleSel();

  protected $preq;
  protected $site;

  public function __construct($form_state, $preq, $site = 'eBayMotors')
  {
    parent::__construct($form_state);
    $this->preq = $preq;
    $this->site = $site;
  }

  protected function getPreq()
  {
    return $this->preq;
  }

  protected function getFormTemplate()
  {
    $isMultipleSel = $this->isMultipleSel();
    $ele = $this->getEleAhah(
      array(
        '#title' => $this->getTitle(),
        '#type' => 'select',
        '#multiple' => $isMultipleSel,
        '#attributes' => array('size' => $isMultipleSel ? 6 : 1),
        '#list_only' => $isMultipleSel,
      )
    );

    if(!$this->isYearDep()){
      $ele['#attributes']['class'] .= ' no-year-dep';
    }

    $isReady = $this->isParentSelected($this->getPreq());

    if(!$isReady){
      $ele['#options'] = array('0' => '-SELECT-');
    }
    else{
      $q = $this->getQuery();
      $ele['#options'] = $q->getSelectOptions($this->getName());
    }

    $dval = $this->getValue(self::prefix . $this->getName());
    $ele['#default_value'] = $dval ? $dval : '0';
    return $ele;
  }
}