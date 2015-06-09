<?php
namespace Oerp\Delivery\Page\Callee;

class Mark_as_READY extends \Oerp\Delivery\Page\Callee\Abstract_mark
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function mark()
  {
    $q = $this->getSelQuery();
    $sels = $this->getSels();

    $q->execute('draft_force_assign', $sels);
    $q->execute('force_assign', $sels);
  }
}