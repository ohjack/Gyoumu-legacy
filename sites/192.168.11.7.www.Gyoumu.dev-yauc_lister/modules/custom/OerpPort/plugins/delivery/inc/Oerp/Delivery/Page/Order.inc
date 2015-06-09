<?php
namespace Oerp\Delivery\Page;

class Order extends \Gulei\Page\Form\_Abstract
{
  private $RecCtrl;
  private $RespCtrl;

  protected function getClassname()
  {
    return __CLASS__;
  }

  public function getRecordCtrl($param = array())
  {
    if($this->RecCtrl){
      return $this->RecCtrl;
    }

    $arch = file_get_contents(__DIR__ . '/Order.xml');
    $param['oerp:ids'] = '[' . $_GET['id'] . ']';

    return $this->RecCtrl =
        new \Oerp\Page\DeliveryOrderRecord($arch, '', $param);
  }

  private function getNoCacheRecord($fld = null)
  {
    $q = new \Oerp\Query\Basic('stock.picking', null, $fld, $_GET['id']);

    $recs = $q->raw(false, true);
    return $recs[0];
  }

  public function getResponseCtrl()
  {
    if(!is_null($this->RespCtrl)){
      return $this->RespCtrl;
    }

    $Resps = array(
      new \Oerp\Delivery\Printer\FedEx\Response(),
      new \Oerp\Delivery\Printer\UPS\Response(),
    );

    $rec = $this->getNoCacheRecord(array('carrier_id'));
    $carr = $rec['carrier_id'][1];

    foreach($Resps as $Resp){
      if($Resp->isApplicable($carr)){
        return $this->RespCtrl = $Resp;
      }
    }
    return $this->RespCtrl = false;
  }

  private function getMsgEle()
  {
    if(!$RespCtrl = $this->getResponseCtrl()){
      return array();
    }

    $flds = array('id', 'name', 'x_tnum');
    $rec = $this->getNoCacheRecord($flds);

    $RespCtrl->check($rec);
    $rec = $this->getNoCacheRecord($flds);

    if($rec['x_tnum'] != '_ERROR_'){
      return array();
    }

    $resp = $RespCtrl->getResp($rec['name']);

    if($resp['success'] === false){
      $src = sprintf('<div class="messages error">%s</div>', $resp['msg']);

      return array(
        'msg' => array(
          '#type' => 'markup',
          '#value' => $src,
        )
      );
    }
    return array();
  }

  protected function hook_form_internal()
  {
    $form = array_merge(
      $this->getMsgEle(),
      $this->getRecordCtrl(array('no-cache' => true))->getFormEntry()
    );
    return $form;
  }

  protected function getToolbarBottom()
  {
    $calls = array('Save');

    if($this->getResponseCtrl()){
      $calls[] = 'Save_Print';
    }
    return $this->getLocalCalleeEle($calls);
  }

  protected function getParam()
  {
    return array();
  }

  public function save($vals)
  {
    $this->getRecordCtrl()->send($vals);
  }
}