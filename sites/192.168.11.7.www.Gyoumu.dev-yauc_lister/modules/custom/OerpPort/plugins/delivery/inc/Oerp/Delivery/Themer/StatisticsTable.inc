<?php
namespace Oerp\Delivery\Themer;

class StatisticsTable implements \Gulei\Themer\_Interface {
  const DB_TABLE = 'oerp_delivery_output';
  protected $axes;
  protected $Iterator;
  protected $IteratorBak;
  protected $count_type = 'product';
  protected $skus;
  protected $skus_total;

  public function __construct($Iterator) {
    $this->IteratorBak = $Iterator;
  }

  public function setAxes($axes) {
    $this->axes = $axes;
  }

  public function setSkus($skus, $skus_total) {
    $this->skus = \Gulei\Text\Helper::explode($skus);
    $this->skus_total = $skus_total;
  }

  public function setCountType($type) {
    $this->count_type = $type;
  }

  protected function renewIterator() {
    $this->Iterator = clone $this->IteratorBak;
  }

  protected function getQtySum($axis = array(), $ex_cri = NULL) {
    $where = $this->Iterator->getWhere();

    $where = array_merge(
      $where, is_array($axis) ? $axis : array($axis));

    if (!is_null($ex_cri)) {
      $where[] = $ex_cri;
    }

    $where = implode(' AND ', $where);

    switch ($this->count_type) {
      case 'product':
        $sql = 'SELECT SUM(qty) AS qty_sum FROM {%s} WHERE %s';
        $sql = sprintf($sql, self::DB_TABLE, $where);

        $data = db_fetch_object(db_query($sql));
        return $data->qty_sum;

      case 'pack':
        $sql = 'SELECT DISTINCT rid FROM {%s} WHERE %s';
        $sql = sprintf($sql , self::DB_TABLE, $where);

        $rs = db_query($sql);
        return $rs->num_rows;
    }
  }

  protected function getStatistics() {
    $axes = $this->axes;
    if (!$axes) {
      $axes = array('(All)' => array());
    }

    $stat = array();
    $ex_cri = NULL;

    if ($skus = $this->skus) {
      if ($this->skus_total) {
        foreach ($skus as &$sku) {
          $sku = '"' . $sku . '"';
        }
        $ex_cri = sprintf('`sku` IN (%s)', implode(', ', $skus));
      }
      else {
        $new_axes = array();
        foreach ($axes as $axis_name => $axis) {
          foreach ($skus as $sku) {
            $new_axis_name = $axis_name . '[' . $sku . ']';

            $new_axis = $axis;
            $new_axis .= sprintf(' AND `sku` = "%s"', $sku);
            $new_axis = '(' . $new_axis . ')';
            $new_axes[$new_axis_name] = $new_axis;
          }
        }
        $axes = $new_axes;
      }
    }

    foreach ($axes as $axis_name => $axis) {
      $this->renewIterator();

      for ($c = 0; $this->Iterator->next(); $c++) {
        if ($ex_cri) {
          $qty_sum = $this->getQtySum($axis, $ex_cri);
        }
        else {
          $qty_sum = $this->getQtySum($axis);
        }

        $col_name = $this->Iterator->getColumnName();
        $stat[$c]['Interval \ Axes'] = $col_name;
        $stat[$c][$axis_name] = $qty_sum ? $qty_sum : 0;
      }
    }
    return $stat;
  }

  public function theme() {
    $stat = $this->getStatistics();
    $hds = array_keys($stat[0]);
    $hds = array_combine($hds, $hds);
    $Tb = new \Gulei\Table\Simple($hds, $stat);
    return $Tb->theme();
  }
}