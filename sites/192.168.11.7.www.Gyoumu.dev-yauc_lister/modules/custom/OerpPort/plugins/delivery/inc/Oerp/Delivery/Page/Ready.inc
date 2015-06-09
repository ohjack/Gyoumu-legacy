<?php
namespace Oerp\Delivery\Page;

class Ready extends \Oerp\Page\Abstract_TreeView
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  private function getOrderCarriers()
  {
    static $cache = null;

    if(!is_null($cache)){
      return $cache;
    }

    $q = new \Oerp\Query\Basic('stock.picking', array(array('state','=','assigned')), array('carrier_id'));
    $cache = array();

    foreach($q->raw() as $rec){
      if($carr = $rec['carrier_id']){
        $cache[$carr[1]][] = $rec['id'];
      }
    }
    return $cache;
  }

  protected function getParam()
  {
    $param = array();
    $orderCarriers = $this->getOrderCarriers();

    if($carr = $_GET['carr']){
      if($ids = $orderCarriers[$carr]){
        $param['criteria'][] = array('id', 'in', $ids);
      }
      else{
        $param['criteria'][] = array('id', '=', 0);
      }
    }

    $param['pager'] = ($carr || $_GET['cri']) ? false : true;
    $param['arch'] = file_get_contents(__DIR__ . '/common.xml');
    $param['criteria'][] = array('state', '=', 'assigned');
    $param['oerp:op-type'] = 'select-many';
    $param['search-custom']
        = array('x_seller', 'name', 'origin', 'carrier_id', 'x_tnum');
    return $param;
  }

  private function getFilterSeller()
  {
    $q = new \Oerp\Query\Basic('stock.picking', array(array('state','=','assigned')), array('x_seller'));
    $filter = array();
    $filter['_all'] = l('All', $_GET['q']);

    foreach($q->raw() as $rec){
      $seller = $rec['x_seller'];

      if($seller && !isset($filter[$seller])){
        $filter[$seller] = l(
          $rec['x_seller'],
          $_GET['q'],
          array(
            'query' => array(
              'cri' => sprintf('[["x_seller","=","%s"]]', $rec['x_seller']))
          )
        );
      }
    }

    $buf = '';
    $buf .= theme('item_list', $filter, 'Filter by seller:', 'ul', array('class' => 'oerp-delivery-filter'));
    $buf .= '<div class="clearfix"></div>';
    return $buf;
  }

  private function getFilterCarrier()
  {
    $orderCarriers = $this->getOrderCarriers();

    $list = array();
    $list[] = l('All', $_GET['q']);

    foreach(array_keys($orderCarriers) as $carr){
      $list[] = l($carr, $_GET['q'], array('query' => array('carr' => $carr)));
    }

    $buf = '';
    $buf .= theme('item_list', $list, 'Filter by carrier:', 'ul', array('class' => 'oerp-delivery-filter'));
    $buf .= '<div class="clearfix"></div>';
    return $buf;
  }

  private function getFilterSrc()
  {
    $buf = '';
    $buf .= $this->getFilterCarrier();
    $buf .= $this->getFilterSeller();
    return '<div class="oerp-delivery-filter-wrapper">' . $buf . '</div>';
  }

  protected function hook_form_internal()
  {
    $form = array();

    $form['filter'] = array(
      '#type' => 'markup',
      '#value' => $this->getFilterSrc()
    );

    $form = array_merge($form, parent::hook_form_internal());
    drupal_add_css(drupal_get_path('module', 'oerp_delivery') . '/oerp_delivery.css');

    return $form;
  }

  protected function getToolbarBottom()
  {
    $calls = array(
      "Oerp\\Delivery\\Page\\Callee\\Download_Ready\\No__track",
      'Print_FedEx',
      'Print_UPS',
      'Mark_as_SENT',
      'Mark_as_OoS',
      "Oerp\\Delivery\\Page\\Callee\\Mark_as_CANCEL",
    );
    return $this->getLocalCalleeEle($calls);
  }
}