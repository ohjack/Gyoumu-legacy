<?php
namespace Yauc\Lister\Page\Queue\Callee;

class Upload extends \Gulei\Page\Form\Callee\_Abstract
{
  protected $seller;

  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function isTrial()
  {
    return false;
  }

  protected function uploadInternal($rec)
  {
    $provRecord = new \Yauc\Lister\Provider\Record($rec['id']);
    $send = $provRecord->getData();

    $mgr = new \Yauc\Driver\Manager($this->seller);
    $mgr->apply(new \Yauc\Driver\Command\uploadAuction(
      array(
        'rec' => $send,
        'isTrial' => $this->isTrial()
      )
    ));
  }

  protected function getRecs()
  {
    $prov = new \Yauc\Lister\Provider\IDs(
      $this->seller,
      array(
        \Yauc\Lister\Provider\_Abstract::Q_NEW,
      )
    );

    return $prov->getData();
  }

  public function execute($form, &$form_state)
  {
    $this->seller = $form['sel_id']['#value'];

    foreach($this->getRecs() as $rec){
      $this->uploadInternal($rec);
    }
  }
}