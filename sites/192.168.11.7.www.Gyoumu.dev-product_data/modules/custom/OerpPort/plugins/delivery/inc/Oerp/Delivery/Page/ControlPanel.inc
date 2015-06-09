<?php
namespace Oerp\Delivery\Page;

class ControlPanel extends \Gulei\Page\Form\_Abstract
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
      "Print_Fedex",
      "Print_UPS",
    );

    $form["prints"] = array_merge(
      array('#themer' => "Gulei\\Page\\Themer\\Panes"),
      $this->getLocalCalleeEle($prints)
    );
/*
    $downloads = array(
      'Download_Ready',
      'Download_Sent',
    );

    $form["downloads"] = array_merge(
      array('#themer' => "Gulei\\Page\\Themer\\Panes"),
      $this->getLocalCalleeEle($downloads)
    );
*/
    return $form;
  }

  protected function getToolbarBottom()
  {
    return array();
  }
}