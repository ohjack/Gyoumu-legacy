<?php
namespace Oerp\Stock\Provider;

class PeriodOutput extends \Gulei\Provider\_Abstract {
  protected function getSuppInfo($cri, $fld) {
    return \Oerp\Query::q('product.supplierinfo', $cri, $fld);
  }

  protected function getProdInfo($ids) {
    return \Oerp\Query::q(
      'product.product',
      NULL,
      array('name', 'default_code', 'product_tmpl_id'),
      $ids
    )->promote('id');
  }

  public function getData() {
    $vals = $this->param;

    if ($vals['supp']) {
      $prod_ids = $this->getSuppInfo(
        array(array('name', 'ilike', $vals['supp'])),
        array('product_id')
      )->collect('product_id', 0);
    }

    if ($vals['prod_mgr']) {
      $prod_ids_by_mgr = \Oerp\Query::q(
        'product.product',
        array(array('product_manager', 'ilike', $vals['prod_mgr'])),
        array('id')
      )->collect('id', 0);

      if ($prod_ids_by_mgr) {
        if ($prod_ids) {
          $prod_ids = array_intersect($prod_ids, $prod_ids_by_mgr);
        }
        else {
          $prod_ids = $prod_ids_by_mgr;
        }
      }
    }

    if (!$prod_ids) return array();

    $moves = \Oerp\Query::q(
      'stock.move',
      array(
        array('state', '=', 'done'),
        array('product_id', 'in', $prod_ids),
        array('date_planned', '>=', $vals['date-from']),
        array('date_planned', '<=', $vals['date-to']),
        array('location_dest_id', 'ilike', 'customers'),
      ),
      array(
        'product_id', 'product_qty',
      )
    );

    $prod_qtys = array();
    foreach ($moves as $move) {
      $prod_qtys[$move['product_id'][0]] += $move['product_qty'];
    }

    $supp_infos = $this->getSuppInfo(
      array(
        array('name', 'ilike', $vals['supp']),
        array('product_id', 'in', array_keys($prod_qtys)),
      ),
      array('name', 'product_id', 'product_code')
    )->promote('product_id', 0);

    $prod_infos = $this->getProdInfo($prod_ids);

    $prod_tmpl_infos = \Oerp\Query::q(
      'product.template', NULL, array('list_price', 'standard_price'),
      \Gulei\_Array::lambda($prod_infos, 'collect', 'product_tmpl_id', 0)
    )->promote('id');

    $send = array();
    foreach ($prod_qtys as $pid => $qty) {
      $prod_info = $prod_infos[$pid];
      $supp_info = $supp_infos[$pid];
      $prod_tmpl_info = $prod_tmpl_infos[$prod_info['product_tmpl_id'][0]];

      $send[$pid] = array(
        'sku' => $prod_info['default_code'],
        'qty' => $qty,
        'name' => $prod_info['name'],
        'sku' => $prod_info['default_code'],
        'pn' => $supp_info['product_code'],
        'list_price' => $prod_tmpl_info['list_price'],
        'standard_price' => $prod_tmpl_info['standard_price'],
      );

      if ($send[$pid]['standard_price'] == 0) {
        $discount = 'N/A';
      }
      else {
        $discount = (float)$send[$pid]['standard_price'] / $send[$pid]['list_price'] * 100;
        $discount = number_format($discount, 2, '.', '') . '%';
      }
      $send[$pid]['discount'] = $discount;
      $send[$pid]['subtotal'] = $send[$pid]['qty'] * $send[$pid]['standard_price'];
    }

    return array_values($send);
  }

  public function count() {
    return count($this->getData());
  }

  public function getReference() {
    return array();
  }
}