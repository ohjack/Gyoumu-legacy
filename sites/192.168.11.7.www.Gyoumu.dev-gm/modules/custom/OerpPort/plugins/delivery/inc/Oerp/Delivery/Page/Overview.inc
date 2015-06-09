<?php
namespace Oerp\Delivery\Page;

class Overview extends \Gulei\Page\Form\_Abstract
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

    $prints = array(
      'Print_Fedex',
//      'Print_UPS'
    );

    $form = array_merge($form, $this->getLocalCalleeEle($prints));
    return $form;
  }

  protected function getToolbarBottom()
  {
    return array();
  }
}