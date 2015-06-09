<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee;

class Download_Ready extends \Oerp\Delivery\Page\ControlPanel\Callee\Abstract_DownloadPanel
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getCallees()
  {
    return array(
      new \Oerp\Delivery\Page\ControlPanel\Callee\Download_Ready\No__track($this->FormPage),
    );
  }
}