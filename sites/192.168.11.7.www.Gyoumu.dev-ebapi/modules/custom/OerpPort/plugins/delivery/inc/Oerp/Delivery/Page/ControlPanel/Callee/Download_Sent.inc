<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee;

class Download_Sent extends \Oerp\Delivery\Page\ControlPanel\Callee\Abstract_DownloadPanel
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getCallees()
  {
    return array(
      new \Oerp\Delivery\Page\ControlPanel\Callee\Download_Sent\No__track_Today($this->FormPage),
      new \Oerp\Delivery\Page\ControlPanel\Callee\Download_Sent\Recently_Sent($this->FormPage),
    );
  }
}