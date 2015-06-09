<?php
namespace Oerp\Delivery\Page\Callee;

class Mark_as_CANCEL extends \Oerp\Delivery\Page\Callee\Abstract_mark
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function mark()
  {
    $q = $this->getSelQuery();
    $sels = $this->getSels();

    foreach($sels as $id){
      $q->exec_workflow(array('button_cancel', $id));
    }
  }
}