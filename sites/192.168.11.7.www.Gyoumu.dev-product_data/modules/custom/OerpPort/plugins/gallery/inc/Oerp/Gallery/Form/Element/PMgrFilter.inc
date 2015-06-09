<?php
namespace Oerp\Gallery\Form\Element;

class PMgrFilter {
  public function getEle($skus, $float = FALSE) {
    $mgr_ids = \Oerp\Query::q(
      'product.product', array(array('default_code', 'in', $skus)), array('product_manager'))
        ->exe('collect', array('product_manager', 0));

    if ($mgr_ids) {
      $mgrs = \Oerp\Query::q('res.users', NULL, array('name', 'login'), $mgr_ids)
          ->promote('login');

      $items = array();
      foreach ($mgrs as $mgr) {
        $items[] = l($mgr['name'], $_GET['q'], array('query' => array('mgr' => $mgr['login'])));
      }

      $ul_class = 'filter';
      $ul_class .= $float ? ' float' : '';

      $src = theme('item_list', $items, 'Filter by manager:', 'ul', array('class' => $ul_class));
      $src = '<div class="filter-wrapper">' . $src . '</div>';
    }
    else {
      $src = '';
    }

    return array(
      '#type' => 'markup',
      '#value' => $src,
    );
  }
}