<?php
namespace Oerp\Delivery\Page;

class OrderFilter extends \Gulei\Page\Form\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function hook_form_internal() {
    $type = $_GET['type'];
    $type_name = str_replace('_', ' ', $type);
    $form = array();

    $q = new \Oerp\Query\Basic('delivery.carrier', null, array('name'));
    $cr_names = (array)$q->raw()->promote('id', true);

    $form['title'] = array(
      '#type' => 'markup',
      '#value' => '<h1>' . $type_name . '</h1>',
    );

    $form['close_time'] = array(
      '#title' => 'Items',
      '#type' => 'radios',
      '#options' => array(
        'before' => 'Before close time',
        'after' => 'After close time',
      ),
      '#default_value' => 'before',
    );

    $form['carrier'] = array(
      '#title' => 'Carrier',
      '#type' => 'checkboxes',
      '#options' => array_combine($cr_names, $cr_names),
      '#checkall' => true,
    );
    return $form;
  }

  protected function getToolbarBottom() {
    $calls = array('Search');
    return $this->getLocalCalleeEle($calls);
  }

  public function getParam() {
    return array();
  }
}