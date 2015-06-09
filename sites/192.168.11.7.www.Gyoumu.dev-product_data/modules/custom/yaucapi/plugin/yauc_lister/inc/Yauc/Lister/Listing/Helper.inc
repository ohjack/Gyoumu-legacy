<?php
namespace Yauc\Lister\Listing;

class Helper extends \Yauc\Driver\_Abstract
{
  protected function createDriver()
  {
    return new \Yauc\Lister\Driver\Basic($this->sel_id, $this->pwd);
  }

  public function upload($isTrial)
  {
    if(!$this->checkDriverReady(true, true)){
      return false;
    }

    $prov = new \Yauc\Lister\Provider\Queue(
      $this->sel_id,
      array(
        \Yauc\Lister\Provider\_Abstract::Q_NEW,
        \Yauc\Lister\Provider\_Abstract::Q_FAILED
      )
    );

    foreach($prov->getData() as $rec){
      if($isTrial){
        $this->driver->submit($rec, true);
        break;
      }
      else{
        $resp = $this->driver->submit($rec);
        $ctrl = new \Yauc\Lister\Query\Queue($rec['id']);

        if($resp === true){
          $ctrl->setDone();
        }
        else{
          $ctrl->setFailed($resp['msg']);
        }
      }
    }
  }
}