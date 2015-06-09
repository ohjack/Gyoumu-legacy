<?php
namespace Gulei\Page\Form\Callee;

abstract class _Abstract extends \Gulei\Page\Form\Component\_Abstract
{
  abstract public function execute($form, &$form_state);

  protected function getEleTpl()
  {
    return array(
      '#type' => 'submit',
      '#value' => $this->getCaption(),
      '#name' => $this->getEleName(),
      '#themer' => $this->getClassname(),
    );
  }

  public function validate($form, &$form_state){}

  public function theme(&$ele)
  {
    return drupal_render($ele);
  }

  protected function reload() {
    $get = $_GET;
    $q = $get['q'];
    unset($get['q']);
    drupal_goto($q, $get);
  }
}