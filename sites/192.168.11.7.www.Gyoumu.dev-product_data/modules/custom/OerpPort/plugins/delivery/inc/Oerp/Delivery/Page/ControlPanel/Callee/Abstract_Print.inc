<?php
namespace Oerp\Delivery\Page\ControlPanel\Callee;

abstract class Abstract_Print extends \Gulei\Page\Form\Callee\_Abstract
{
  abstract protected function getCarrierNames();
  abstract protected function getPrinterCall();
  abstract protected function getPrinterResp();
  abstract protected function getWaitingCallee();

  public function execute($form, &$form_state)
  {
    $prov = $this->createProvider('Printable');
    $Call = $this->getPrinterCall();
    $Call->printOrders($prov);
  }

  protected function getReportEle($prov, $name, $ex_callee)
  {
    $ele_name = strtolower($name);

    $ele[$ele_name] = array(
      '#type' => 'fieldset',
      '#title' => $name . '(' . $prov->count() . ')',
      '#collapsible' => true,
      '#collapsed' => true,
    );

    $report = '';
    foreach($prov->getData() as $rec){
      $oname = sprintf('%s [%s] - %s', $rec['name'], $rec['x_tnum'], $rec['x_seller']);

      $report .= l(
        $oname, 'oerp/delivery/order',
        array(
          'query' => array('id' => $rec['id']),
          'attributes' => array('target' => '_blank')
        )
      );
      $report .= '<br/>';
    }
    $report = '<div>' . $report . '</div>';

    $ele[$ele_name]['report'] = array(
      '#type' => 'markup',
      '#value' => $report,
    );

    if($ex_callee){
      $ele[$ele_name]['ex_callee'] = $this->FormPage->getLocalCalleeEle($ex_callee);
    }

    return $ele;
  }

  public function getEleTpl()
  {
    $provWaiting = $this->createProvider('Waiting');

    $recs = $provWaiting->getData();
    $Resp = $this->getPrinterResp();
    $Resp->check($recs);

    $ele = array();
    $ele['button'] = $this->getButtonEle();

    $reports = array(
      'Waiting' => array(
        'prov' => $provWaiting,
        'ex_callee' => $this->getWaitingCallee(),
      ),
      'Error' => array(
        'prov' => $this->createProvider('Error'),
      ),
      'Printed' => array(
        'prov' => $this->createProvider('Printed'),
      ),
    );

    foreach($reports as $rep_name => $param){
      $ele = array_merge(
        $ele, $this->getReportEle($param['prov'], $rep_name, $param['ex_callee']));
    }

    $ele['#themer'] = "Gulei\\Page\\Themer\\Pane";
    return $ele;
  }

  private function getButtonEle()
  {
    $tpl = parent::getEleTpl();

    $prov = $this->createProvider('Printable');
    $count = $prov->count();

    $over = array(
      '#value' => $this->getCaption() . ' (' . $count . ')',
    );

    if(!$count){
      $over['#attributes'] = array('disabled' => 'disabled');
    }
    return array_merge($tpl, $over);
  }

  protected function createProvider($type)
  {
    $flds = array('id', 'name', 'x_seller', 'x_tnum');
    $h = new \Oerp\Delivery\Provider\Ready\Helper();

    return $h->createProvider(
      $this->getCarrierNames(), $type, $flds
    );
  }
}