<?php
namespace Oerp\Delivery\Page\Ready\Callee;

class Mark_as_OoS extends \Oerp\Delivery\Page\Callee\Abstract_mark
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function mark()
  {
    $q = $this->getSelQuery();
    $q->write(array('x_tnum' => ''));
    $q->execute('cancel_assign', $this->getSels());
  }
}