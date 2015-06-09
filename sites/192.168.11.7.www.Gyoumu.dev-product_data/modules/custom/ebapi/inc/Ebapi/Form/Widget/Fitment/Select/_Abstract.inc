<?php
namespace Ebapi\Form\Widget\Fitment\Select;

abstract class _Abstract extends \Ebapi\Form\Widget\Fitment\_Abstract
{
  abstract static protected function getName();
  abstract protected function getTitle();
  abstract protected function isInYearRange();
  abstract protected function isYearDep();

  protected $preq;

  public function __construct($form_state, $preq)
  {
    parent::__construct($form_state);
    $this->preq = $preq;
  }

  protected function getPreq()
  {
    return $this->preq;
  }

  protected function getFormTemplate()
  {
    $ele = $this->getEleAhah(
      array(
           '#title' => $this->getTitle(),
           '#type' => 'select',
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
      $q = new \Ebapi\Query\Fitment(
        $this->getCri($this->getPreq()),
        $this->isInYearRange() ? $this->getYearRange() : null
      );

      $ele['#options'] = $q->getSelectOptions($this->getName());
    }

    $dval = $this->getValue(self::prefix . $this->getName());
    $ele['#default_value'] = $dval ? $dval : '0';

    return $ele;
  }
}