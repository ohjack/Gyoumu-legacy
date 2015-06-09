<?php
namespace Yauc\Lister\Query;

class Queue extends \Yauc\Lister\Provider\_Abstract
{
  public function __construct($id)
  {
    $this->id = $id;
  }

  public function count()
  {
    return 1;
  }

  private function setStatus($status, $msg = null)
  {
    $newRec = new \stdClass();
    $newRec->id = $this->id;
    $newRec->status = $status;

    if(!is_null($msg)){
      $newRec->msg = $msg;
    }
    \drupal_write_record(self::TABLENAME, $newRec, 'id');
  }

  public function setFailed($msg)
  {
    $this->setStatus(\Yauc\Lister\Provider\_Abstract::Q_FAILED, ($msg) ? $msg : 'Unkown error.');
  }

  public function setDone()
  {
    $this->setStatus(\Yauc\Lister\Provider\_Abstract::Q_DONE, '');
  }

  public function setCancel()
  {
    $this->setStatus(\Yauc\Lister\Provider\_Abstract::Q_CANCEL);
  }

  public function getData()
  {
    $recs = parent::getData();
    return $recs[0];
  }

  public function checkCategory()
  {
    $rec = $this->getData();
    $ctrl = new \Yauc\Lister\Listing\Helper($rec['sellerId']);

    if($ctrl->checkDriverReady(true, true)){
      $drv = $ctrl->getDriver();
      return $drv->checkCategory($rec['category']);
    }
    return false;
  }
}