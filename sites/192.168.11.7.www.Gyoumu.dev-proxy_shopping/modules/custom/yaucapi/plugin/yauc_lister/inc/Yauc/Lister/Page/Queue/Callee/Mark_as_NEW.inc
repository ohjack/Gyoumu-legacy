<?php
namespace Yauc\Lister\Page\Queue\Callee;

class Mark_as_NEW extends \Yauc\Lister\Page\Queue\Callee\Abstract_Mark_as
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function markAs($id)
  {
    $c = new \Yauc\Lister\Query\Queue($id);
    $c->setNew();
  }

  protected function getMsgPattern()
  {
    return 'Set %s record(s) as NEW.';
  }
}