<?php
namespace Oerp\Delivery\Page;

class Cancel extends \Gulei\Page\Form\_Abstract {
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function hook_form_internal()
  {
    return array();
  }

  protected function getToolbarBottom()
  {
    return array();
  }

  protected function getParam()
  {
    return array();
  }
}