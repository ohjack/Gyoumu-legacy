<?php
namespace Oerp\Delivery\Themer;

class StatisticsTable implements \Gulei\Themer\_Interface {
  const DB_TABLE = 'oerp_delivery_output';
  const TB_COLCAP = 'Interval \ Axes';
  protected $axes;
  protected $Iterator;
  protected $IteratorBak;
  protected $count_type = 'product';
  protected $skus;
  protected $skus_total;
  protected $rows_for = 'axis';

  public function __construct($Iterator) {
    $this->IteratorBak = $Iterator;
  }

  public function setAxes($axes) {
    $this->axes = $axes;
  }

  public function setRowsFor($rows_for) {
    $this->rows_for = $rows_for;
  }

  public function setSkus($skus, $skus_total) {
    $skus_src = \Gulei\Text\Helper::explode($skus);
    $this->skus = $skus_src;
/*
    if ($skus_src) {
      $where = array();
      foreach ($skus_src as $sku_src) {
        $where[] = "`sku` LIKE '$sku_src%'";
      }
      $where = implode(' OR ', $where);
      $query = sprintf(
        'SELECT DISTINCT `sku` FROM {oerp_delivery_output} WHERE %s',
        $where
      );

      $rs = db_query($query);
      $this->skus = array();

      while ($data = db_fetch_object($rs)) {
        $this->skus[] = $data->sku;
      }
    }
*/
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

    if ($axis) {
      $where = array_merge(
        $where, is_array($axis) ? $axis : array($axis));
    }

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
      $axes = array('(All)' => '');
    }
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

            $new_axis = $axis ? $axis . ' AND' : '';
            $new_axis .= sprintf(' `sku` = "%s"', $sku);
            $new_axis = '(' . $new_axis . ')';
            $new_axes[$new_axis_name] = $new_axis;
          }
        }
        $axes = $new_axes;
      }
    }

    $stat = array();
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
        $stat[$c][self::TB_COLCAP] = $col_name;
        $stat[$c][$axis_name] = $qty_sum ? $qty_sum : 0;
      }
    }
    return $stat;
  }

  protected function prepareRecs($stats) {
    switch ($this->rows_for) {
      case 'interval':
        return $stats;

      case 'axis':
        $recs = array();
        foreach ($stats as $stat) {
          foreach ($stat as $axis_name => $axis_val) {
            if ($axis_name == self::TB_COLCAP) {
              continue;
            }
            $recs[$axis_name]['Axe \ Interval'] = $axis_name;
            $recs[$axis_name][$stat[self::TB_COLCAP]] = $axis_val;
          }
        }
        return array_values($recs);
    }
  }

  public function getTable() {
    $stat = $this->getStatistics();
    $recs = $this->prepareRecs($stat);
    $hds = array_keys($recs[0]);
    $hds = array_combine($hds, $hds);
    return new \Gulei\Table\Simple($hds, $recs);
  }

  public function theme() {
    $Tb = $this->getTable();
    return $Tb->theme(NULL, NULL, array(
      'attrs' => array('id' => 'OutputStatistics-result')
    ));
  }
}