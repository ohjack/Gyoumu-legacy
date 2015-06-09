<?php
namespace Oerp\Purchase\Provider;

class MoveHistoryByDay extends \Gulei\Provider\_Abstract {
  protected $year;
  protected $month;
  protected $day;

  protected function getDateToks($date_str) {
    return explode('-', $date_str);
  }

  protected function init() {
    $date = $this->param['date-from'];
    $date = $date ? $date : date('Y-m-d', mktime());
    list($this->year, $this->month, $this->day) = $this->getDateToks($date);
  }

  public function getDateStr() {
    return sprintf(
      '%s%s',
      $this->param['date-from'],
      ($to = $this->param['date-to']) ? ' ~ ' . $to : ''
    );
  }

  protected function explodeDate($date) {
    $toks = explode(' ', $date);
    return $toks[0];
  }

  protected function getDate($Y, $m, $d, $H, $i, $s) {
    return date('Y-m-d H:i:s', mktime($H, $i, $s, $m, $d, $Y));
  }

  protected function getDateCriTo() {
    $date_to = $this->param['date-to'];

    if (!$date_to) {
      return $this->getDate($this->year, $this->month, $this->day, 23, 59, 59);
    }

    $toks = $this->getDateToks($date_to);
    return $this->getDate($toks[0], $toks[1], $toks[2], 23, 59, 59);
  }

  protected function getDateCri() {
    $date_from = $this->getDate($this->year, $this->month, $this->day, 0, 0, 0);
    $date_to = $this->getDateCriTo();

    return array(
      array($this->param['date-type'], '>=', $date_from),
      array($this->param['date-type'], '<=', $date_to),
    );
  }

  protected function isDateIn($date) {
    $cri = $this->getDateCri();
    return strcmp($date, $cri[0][2]) >= 0
        && strcmp($date, $cri[1][2]) <= 0;
  }

  protected function getPoLineIds() {
    $cri = array(
      array('state', '=', 'done'),
      array('location_id', '=', 7),
      array('location_dest_id', '=', 11),
      array('product_qty', '>', 0),
    );

    $cri = array_merge($cri, $this->getDateCri());

    $ids = \Oerp\Query::q(
      'stock.move',
      $cri,
      array('purchase_line_id')
    )->collect('purchase_line_id', 0);

    $ids = array_filter($ids);
    $ids = array_values($ids);
    return $ids;
  }

  protected function getRawData() {
    $po_line_ids = $this->getPoLineIds();

    if (!$po_line_ids) {
      return array();
    }
    else {
      if ($supp = $this->param['supp']) {
        $supp_po_ids = \Oerp\Query::q(
          'purchase.order',
          array(
            array('partner_id', 'ilike', $supp),
          ),
          array('id')
        )->collect('id');

        $cri = array(array('order_id', 'in', $supp_po_ids));
      }
      else {
        $cri = NULL;
      }

      $po_ids = \Oerp\Query::q(
        'purchase.order.line',
        $cri,
        array('order_id'),
        $po_line_ids
      )->collect('order_id', 0);

      if (!$po_ids) {
        return array();
      }

      $prov = new \Oerp\Purchase\Provider\PoLines($po_ids);
      $pos = (array)$prov->getData();

      $po_ids = \Gulei\_Array::lambda($pos, 'collect', 'id');
      $prov = new \Oerp\Purchase\Provider\PoPicking($po_ids);
      $picks = $prov->getData();

      $supp_ref = array();

      foreach ($pos as $po) {
        $supp_ref['supp_ids'][] = $po['partner_id']['id'];

        foreach ($po['order_line'] as $oline) {
          $supp_ref['prod_ids'][] = $oline['product_id']['id'];
        }
      }

      $cri = array(
        array('name', 'in', $supp_ref['supp_ids']),
        array('product_id', 'in', $supp_ref['prod_ids'])
      );

      $q = new \Oerp\Query(
        'product.supplierinfo',
        $cri,
        array('name', 'product_id', 'product_code')
      );

      $pcodes = array();
      foreach ($q->raw() as $rec) {
        $pcodes[$rec['name'][0]][$rec['product_id'][0]] = $rec['product_code'];
      }

      $states = \Oerp\Query\Fields::getDef('purchase.order');
      $states = $states['state']['selection'];

      foreach ($pos as $po_ind => &$po) {
        foreach ($po['order_line'] as $sku => &$oline) {
          if (!in_array($oline['id'], $po_line_ids, TRUE)) {
            unset($pos[$po_ind]['order_line'][$sku]);
            continue;
          }

          $sum = 0;

          foreach ($oline['move_ids'] as $mv_ind => &$move) {
            if ($move['state'] != 'done'
                || $move['product_qty'] < 1
                || !$this->isDateIn($move[$this->param['date-type']])
            ) {
              unset($oline['move_ids'][$mv_ind]);
              continue;
            }

            if ($move['location_dest_id'] == 'Suppliers') {
              $move['product_qty'] *= -1;
            }

            $sum += $move['product_qty'];
            $move['date'] = $this->explodeDate($move['date']);
            $move['date_planned'] = $this->explodeDate($move['date_planned']);
          }

          $oline['move_ids'] = array_values($oline['move_ids']);

          if ($this->param['sum']) {
            unset($po['order_line'][$sku]['move_ids']);
          }

          $oline['func:picked'] = '/ ' . $picks[$po['name']][$sku]['done'];
          $oline['func:sum'] = $sum;
          $oline['func:subtotal'] = $sum * $oline['price_unit'];

          $oline['product_id']['func:pcode'] =
              $pcodes[$po['partner_id']['id']][$oline['product_id']['id']];
        }

        $po['state'] = $states[$po['state']];

        $funcSort = create_function('$a,$b', '
          return substr($a, 3) > substr($b, 3);
        ');

        uksort($po['order_line'], $funcSort);
        $po['order_line'] = array_values($po['order_line']);
        unset($pos[$po_ind]['picking_ids']);
      }
    }

    $func_sort_po = create_function('$a,$b', '
      return substr($a["name"], 2) > substr($b["name"], 2);
    ');

    usort($pos, $func_sort_po);
    return $pos;
  }

  public function getData() {
    $arch = file_get_contents(__DIR__ . '/MoveHistoryByDay_supplement.xml');
    return \Oerp\Util\Helper::convNested2Flat(
      $this->getRawData(), $arch, $this->param['copy_method']);
  }

  public function count() {
    return count($this->getData());
  }

  public function getReference() {
    return array();
  }
}
