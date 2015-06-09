<?php
namespace Oerp\Delivery\Page\Ready\Callee;

class Mark_as_SENT extends \Oerp\Delivery\Page\Callee\Abstract_mark
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function mark()
  {
    $q = $this->getSelQuery();
    $q->execute('draft_validate', $this->getSels());
  }
}