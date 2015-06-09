<?php
namespace Oerp\Delivery\Page;

class ReportPrint extends \Gulei\Page\Form\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  static public function getMenuEntry($type) {
    $name = str_replace('_', ' ', $type);
    return array(
      l($name, 'gulib/form',
        array(
          'query' => array(
            'n' => 'Oerp:Delivery:Page:OrderFilter',
            'type' => $type,
          ),
        )
      ),
    );
  }

  protected function getCarrierIds() {
    $carrs = explode('|', $_GET['carrier']);
    $cri = array(
      array('name', 'in', $carrs),
    );

    $q = new \Oerp\Query('delivery.carrier', $cri, array('id'));
    return $q->raw()->collect('id');
  }

  protected function hook_form_internal() {
    $type = $_GET['type'];
    $prov_name = "Oerp\\Delivery\\Provider\\ReportPrint\\" . $type;
    $Prov = new $prov_name();

    $form['list'] = array(
      '#type' => 'markup',
      '#value' => $Prov->getThemeSrc(),
    );

    \Oerp\Util\Helper::addCss();
    return $form;
 }

  protected function getToolbarBottom() {
    return array();
  }

  public function getParam() {
    return array();
  }
}