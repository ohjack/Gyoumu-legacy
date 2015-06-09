<?php
namespace Oerp\Page;

class DeliveryOrderRecord extends \Oerp\Query\Record
{
  private $carrier;

  public function getRecord($param = array())
  {
    $rec = parent::getRecord($param);
    $this->carrier = $rec['carrier_id']['name'];
/*
    $UpsPrintCtrl = new \Oerp\Printer\UPS($rec['name']);

    if(!$rec['x_tnum'] && $UpsPrintCtrl->isPrint()){
      if($msg = $UpsPrintCtrl->getLogMessage()){
        drupal_set_message(nl2br($msg), 'error', false);
      }
    }
*/
    return $rec;
  }

  public function getFormEntry()
  {
    $form = array();
    $form['details'] = parent::getFormEntry();
    $form['details']['#width'] = 'large';
    $form['RecObj'] = array(
      '#type' => 'value',
      '#value' => $this->save(),
    );
    return $form;
  }
}