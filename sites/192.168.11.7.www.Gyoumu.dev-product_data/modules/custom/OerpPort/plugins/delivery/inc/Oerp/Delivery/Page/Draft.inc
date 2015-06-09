<?php
namespace Oerp\Delivery\Page;

class Draft extends \Gulei\Page\Form\Abstract_WithArch
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getDirPath()
  {
    return __DIR__;
  }

  protected function getParam()
  {
    return array();
  }

  protected function getEntryName()
  {
    return 'order_list';
  }

  protected function getToolbarBottom()
  {
    $calls = array(
      'Mark_as_READY',
      'Delete',
    );
    return $this->getLocalCalleeEle($calls);
  }
}