<?php
namespace Oerp\Delivery\Provider\Statistics;

abstract class _Abstract extends \Gulei\Provider\_Abstract {
  const DB_TABLE = 'oerp_delivery_output';

  abstract protected function getType();

  protected function getDistinctVals($col) {
    $sql = 'SELECT DISTINCT `%s` FROM {%s} ORDER BY country ASC';
    $rs = db_query($sql, $col, self::DB_TABLE);

    $vals = array();
    while ($val = db_result($rs)) {
      $vals[] = $val;
    }
    return $vals;
  }

  protected function getValues() {
    $cs = array_fill_keys($this->getDistinctVals($this->getType()), NULL);

    $sql = 'SELECT COUNT(*) FROM {%s} WHERE `%s`="%s"';
    foreach (array_keys($cs) as $c) {
      $rs = db_query($sql, self::DB_TABLE, $this->getType(), $c);
      $cs[$c] = db_result($rs);
    }

    arsort($cs);
    return array_keys($cs);
  }

  public function getData() {
    $vals = $this->getValues();
    $opts = array_combine($vals, $vals);
    return $opts;
  }

  public function count() {
    return count($this->getData());
  }

  public function getReference() {
    return array();
  }
}