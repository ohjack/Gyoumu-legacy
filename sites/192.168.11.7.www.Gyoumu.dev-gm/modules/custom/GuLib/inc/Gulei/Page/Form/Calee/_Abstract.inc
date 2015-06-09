<?php
namespace Gulei\Page\Form\Calee;

abstract class _Abstract
{
  protected $FormPage;

  abstract protected function getClassname();
  abstract public function execute($form, &$form_state);

  public function __construct(\Gulei\Page\Form\_Abstract $FormPage)
  {
    $this->FormPage = $FormPage;
  }

  public function getEleName()
  {
    return 'ns:' . str_replace("\\", ':', $this->getClassname());
  }

  protected function getCaption()
  {
    $toks = explode("\\", $this->getClassname());
    return str_replace('_', ' ', array_pop($toks));
  }

  protected function getEleOverrides()
  {
    return array();
  }

  public function getEle()
  {
    $tpl = array(
      '#type' => 'submit',
      '#value' => $this->getCaption(),
      '#name' => $this->getEleName(),
    );

    return array_merge(
      $tpl, $this->getEleOverrides()
    );
  }

  public function validate($form, &$form_state){}
}