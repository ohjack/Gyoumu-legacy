<?php
namespace Oerp\Delivery\Page\Overview\Callee;

abstract class Abstract_Print extends \Gulei\Page\Form\Calee\_Abstract
{
  abstract protected function getPrinterCall($prov);
  abstract protected function getPrinterResp($prov);
  abstract protected function createProvider($type);
  abstract protected function getWaitingCallee();

  public function execute($form, &$form_state)
  {
    $Call = $this->getPrinterCall($this->createProvider('Printable'));
    $Call->printOrders();
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
      $report .= l(
        $rec['name'], 'oerpport/delivery/order',
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

  public function getEle()
  {
    $provWaiting = $this->createProvider('Waiting');
    $Resp = $this->getPrinterResp($provWaiting);
    $Resp->check();

    $ele = array();
    $ele['button'] = array_merge(parent::getEle());

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
    return $ele;
  }

  protected function getEleOverrides()
  {
    $count = $this->createProvider('Printable')->count();
    $over = array(
      '#value' => $this->getCaption() . ' (' . $count . ')',
    );

    if(!$count){
      $over['#attributes'] = array('disabled' => 'disabled');
    }
    return $over;
  }
}