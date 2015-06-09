<?php
namespace Oerp\Delivery\Page;

class RakutenTracking extends \Gulei\Page\Form\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function hook_form_internal() {
    $form = array();

    $form['page_src'] = array(
      '#type' => 'textarea',
      '#title' => 'Rakuten Source',
    );

    $form['date'] = array(
      '#type' => 'date',
      '#title' => 'Sent Date',
    );

    return $form;
  }

  protected function getToolbarBottom() {
    $calls = array('Download');
    return $this->getLocalCalleeEle($calls);
  }

  public function getParam() {
    return array();
  }
}