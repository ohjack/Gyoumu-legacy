<?php
namespace Oerp\Delivery\Page;

class OutputStatistics extends \Gulei\Page\Form\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function getAxisOptions($type) {
    $classname =
        "Oerp\\Delivery\\Provider\\Statistics\\" . ucfirst($type) . 'Options';
    $prov = new $classname();
    return $prov->getData();
  }

  protected function getAxisType() {
    $axis_type = $_GET['axis'];
    return $axis_type ? $axis_type : 'country';
  }

  protected function getAxisEle($vals) {
    $axis_type = $this->getAxisType();
    $key_name = 'axis-' . $axis_type;

    $ele = array();
    $ele[$key_name] = array(
      '#title' => ucfirst($axis_type),
      '#type' => 'checkboxes',
      '#options' => $this->getAxisOptions($axis_type),
      '#default_value' => is_array(
        $vals[$key_name]) ? $vals[$key_name] : array(),
      '#checkall' => TRUE,
      '#prefix' => '<div class="axis-wrapper">',
      '#suffix' => '</div><div style="clear:both;"></div>',
    );
    return $ele;
  }

  protected function setThemerAxis($vals, $Themer) {
    $axis_type = $this->getAxisType();

    if ($axis_vals = array_filter($vals['axis-' . $axis_type])) {
      $axes = array();
      foreach ($axis_vals as $axis_val) {
        $axes[$axis_val] = $axis_type . '="' . $axis_val . '"';
      }
      $Themer->setAxes($axes);
    }
  }

  protected function getAxisNav() {
    $query = $_GET;
    unset($query['q']);
    $q = $_GET['q'];

    $axis_types = array('country', 'seller', 'carrier');
    $items = array();

    foreach ($axis_types as $axis_type) {
      $query['axis'] = $axis_type;
      $items[] = l('By ' . $axis_type, $q, array('query' => $query));
    }

    $nav_src = '<h2>Axis Type</h2>';
    $nav_src .= theme('item_list', $items);
    return $nav_src;
  }

  protected function hook_form_internal() {
    $CtrlState = new \Gulei\Form\State($this->form_state);
    $vals = $CtrlState->getVals();

    $form = array();

    $form['axis-type-nav'] = array(
      '#type' => 'markup',
      '#value' => $this->getAxisNav(),
    );

    $form['axis'] = $this->getAxisEle($vals);

    $form['date-from'] = array(
      '#title' => 'Date From',
      '#type' => 'date',
      '#default_value' => $vals['date-from'],
    );

    $form['date-to'] = array(
      '#title' => 'Date To',
      '#type' => 'date',
      '#default_value' => $vals['date-to'],
    );

    $form['interval'] = array(
      '#title' => 'Interval',
      '#type' => 'radios',
      '#options' => array(
        'Seasonly' => 'Seasonly',
        'Monthly' => 'Monthly',
        'Weekly' => 'Weekly',
        'Daily' => 'Daily',
      ),
      '#default_value' => $vals['interval'] ? $vals['interval'] : 'Weekly',
    );

    $form['skus'] = array(
      '#title' => 'Products',
      '#type' => 'textarea',
      '#default_value' => $vals['skus'],
    );

    $dval_skus_total = $vals['skus-total'];

    $form['skus-total'] = array(
      '#title' => 'Total',
      '#type' => 'checkbox',
      '#default_value' => is_null($dval_skus_total) ? TRUE : $dval_skus_total,
    );

    $form['query'] = $this->getLocalCalleeEle('Query');

    if ($vals) {
      $date_from = $vals['date-from'];
      $date_to = $vals['date-to'];

      $cname = "Oerp\\Delivery\\Iterator\\" . $vals['interval'];

      $Iterator = new $cname(
        $date_from['year'], $date_from['month'], $date_from['day']);

      $Iterator->setEndTime(
        $date_to['year'], $date_to['month'], $date_to['day']);

      $Themer = new \Oerp\Delivery\Themer\StatisticsTable($Iterator);
      $Themer->setSkus($vals['skus'], $vals['skus-total']);
      $this->setThemerAxis($vals, $Themer);

      $form['view'] = array(
        '#type' => 'markup',
        '#value' => $Themer->theme(),
      );
    }

    drupal_add_css(drupal_get_path('module', 'oerp_delivery') . '/oerp_delivery.css');
    return $form;
  }

  protected function getToolbarBottom() {
    return array();
  }

  public function getParam() {
    return array();
  }
}