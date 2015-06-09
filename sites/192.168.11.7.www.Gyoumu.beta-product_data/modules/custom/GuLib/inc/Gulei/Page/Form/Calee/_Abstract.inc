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

  public function getEle()
  {
    return array(
      '#type' => 'submit',
      '#value' => $this->getCaption(),
      '#name' => $this->getEleName(),
    );
  }
}