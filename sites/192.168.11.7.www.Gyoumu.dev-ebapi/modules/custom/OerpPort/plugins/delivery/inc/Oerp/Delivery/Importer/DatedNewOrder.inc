<?php
namespace Oerp\Delivery\Importer;

class DatedNewOrder extends \Oerp\Delivery\Importer\NewOrder {
  protected function getLockDate() {
    $date = array();
    foreach (array('year', 'month', 'day') as $fld) {
      if ($fld) {
        $date[$fld] = $_GET[$fld];
      }
    }
    return $date;
  }

  public function getEles($form_state) {
    $eles = array();
    $eles['date'] = array(
      '#type' => 'date',
    );

    $date = $form_state['values']['date'];
    if ($date) {
      $eles['date']['#default_value'] = $date;
    }

    if ($date = $this->getLockDate()) {
      if (!$eles['date']['#default_value']) {
        $eles['date']['#default_value'] = $date;
      }
      $eles['date']['#disabled'] = TRUE;
    }
    return $eles;
  }

  protected function getRecDate() {
    $date = $this->StateCtrl->getVal('date');
    $date = date(
      'Y-m-d H:i:s',
      mktime(0, 0, 0, $date['month'], $date['day'], $date['year'])
    );
    return $date;
  }

  public function theme($form) {
    $buf = '';
    $buf .= drupal_render($form['header']);
    $buf .= drupal_render($form['csv_file']);
    $buf .= drupal_render($form['ex_ele']);
    $buf .= drupal_render($form);
    return $buf;
  }
}