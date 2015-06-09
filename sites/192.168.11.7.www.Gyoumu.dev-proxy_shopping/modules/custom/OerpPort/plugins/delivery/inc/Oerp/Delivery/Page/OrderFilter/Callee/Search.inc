<?php
namespace Oerp\Delivery\Page\OrderFilter\Callee;

class Search extends \Gulei\Page\Form\Callee\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  public function execute($form, &$form_state) {
    $carriers = array_filter($form_state['values']['carrier']);
    $query = $_GET;
    $query['carrier'] = implode('|', $carriers);
    $query['after'] = (int) ($form_state['values']['close_time'] == 'after');
    $query['q'] = 'print/' . $query['q'];
    $query['n'] = 'Oerp:Delivery:Page:ReportPrint';
    drupal_goto('', $query);
  }
}