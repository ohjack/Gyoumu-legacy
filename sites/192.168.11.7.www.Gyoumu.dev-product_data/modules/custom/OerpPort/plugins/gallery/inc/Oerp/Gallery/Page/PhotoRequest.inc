<?php
namespace Oerp\Gallery\Page;

class PhotoRequest extends \Gulei\Page\Form\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    return array();
  }

  protected function hook_form_internal()
  {
    $form = array();

    $form['input'] = array(
      '#title' => 'Request',
      '#type' => 'textarea',
      '#description' => 'Format: SKU + Note',
    );

    $h = new \Oerp\Gallery\Helper();

    $form['queued'] = array(
      '#type' => 'markup',
      '#value' => $h->getInputStatus(),
    );
    return $form;
  }

  protected function getToolbarBottom()
  {
    $callees = array('Submit');
    return $this->getLocalCalleeEle($callees);
  }
}