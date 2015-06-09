<?php
namespace Ebapi\Query;

abstract class AbstractFitment
{
  protected $cri;
  protected $yrange;

  public function __construct($cri, $yrange)
  {
    $this->cri = $cri;
    $this->yrange = $yrange;
  }

  abstract protected function sqlGetRecs($flds, $order_by);

  abstract protected function getFitmentIdFld();

  abstract protected function getFitmentMainDisplay();

  abstract protected function getFitmentSubDisplay();

  abstract protected function getWhereForYearRange();

  abstract protected function sqlRecsDistinct($fld, $where);

  public function getWhere()
  {
    return $this->_getWhere($this->cri);
  }

  protected function getListId($rec) {
    return $rec->{$this->getFitmentIdFld()};
  }

  protected function getPreviewText($rec) {
    return $rec->{$this->getFitmentMainDisplay()} . ' ' . $rec->{$this->getFitmentSubDisplay()};
  }

  public function getFitmentList()
  {
    $recs = $this->getRecs(
      $this->getFitmentFlds(),
      $this->getFitmentMainDisplay()
    );

    $list = array();
    foreach ($recs as $rec) {
      $list[$this->getListId($rec)] = $this->getPreviewText($rec);
    }
    return $list;
  }

  public function getRecs($flds, $order_by)
  {
    $rs = db_query($this->sqlGetRecs($flds, $order_by));
    $recs = array();

    while ($data = db_fetch_object($rs)) {
      $recs[] = $data;
    }
    return $recs;
  }

  public function getSelectOptions($name)
  {
    $where = $this->getWhereForOpts();
    $opts = array('0' => '-SELECT-');
    $recs = $this->getRecsDistinct($name, $where);

    foreach ($recs as $rec) {
      $opts[$rec] = $rec;
    }
    return $opts;
  }

  public function getWhereForOpts()
  {
    $cri = array('Make' => $this->cri['Make']);
    return $this->_getWhere($cri);
  }

  protected function getFitmentFlds()
  {
    return array(
      $this->getFitmentIdFld(),
      $this->getFitmentMainDisplay(),
      $this->getFitmentSubDisplay()
    );
  }

  protected function getFldMap() {
    return array();
  }

  protected function getIgnoreFldInWhere() {
    return array();
  }

  protected function mapFldNameFromResp($fld) {
    $map = $this->getFldMap();
    if (isset($map[$fld])) {
      return $map[$fld];
    }
    return $fld;
  }

  protected function _getWhere($cri)
  {
    $where = array();

    foreach ($cri as $key => $val) {
      if (in_array($key, $this->getIgnoreFldInWhere(), true)) {
        continue;
      }

      $key = $this->mapFldNameFromResp($key);

      if (!is_array($val)) {
        $val = array($val);
      }
      if ($val[0]) {
        $ins = array();
        foreach ($val as $sel) {
          $ins[] = "'$sel'";
        }
        if (count($ins) > 1) {
          $where[] = sprintf('`%s` IN (%s)', $key, implode(',', $ins));
        } else {
          $where[] = sprintf('`%s`=%s', $key, $ins[0]);
        }
      }
    }

    if ($this->yrange) {
      $where_yrange = $this->getWhereForYearRange();
      if ($where_yrange) {
        $where[] = $where_yrange;
      }
    }

    if (empty($where)) {
      $where = '';
    } else {
      $where = 'WHERE ' . implode($where, ' AND ');
    }
    return $where;
  }

  protected function getRecsDistinct($fld, $where)
  {
    $rs = db_query($this->sqlRecsDistinct($fld, $where));
    $recs = array();

    while ($data = db_result($rs)) {
      $recs[] = $data;
    }
    return $recs;
  }
}