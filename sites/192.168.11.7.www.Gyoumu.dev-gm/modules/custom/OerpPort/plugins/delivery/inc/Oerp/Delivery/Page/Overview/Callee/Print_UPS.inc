<?php
namespace Oerp\Delivery\Page\Overview\Callee;

class Print_UPS extends \Oerp\Delivery\Page\Overview\Callee\Abstract_Print
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getProvider()
  {
    return new \Oerp\Delivery\Provider\Ready\UPS();
  }

  protected function getWaitingReport()
  {
    return '';
  }

  public function execute($form, &$form_state)
  {
  }
}