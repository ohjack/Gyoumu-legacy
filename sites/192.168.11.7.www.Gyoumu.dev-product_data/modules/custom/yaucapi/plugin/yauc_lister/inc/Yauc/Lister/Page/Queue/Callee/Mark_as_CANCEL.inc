<?php
namespace Yauc\Lister\Page\Queue\Callee;

class Mark_as_CANCEL extends \Yauc\Lister\Page\Queue\Callee\Abstract_Mark_as
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function markAs($id)
  {
    $c = new \Yauc\Lister\Query\Queue($id);
    $c->setCancel();
  }

  protected function getMsgPattern()
  {
    return 'Canceled %s queued record(s).';
  }
}