<?php
namespace Ebapi\Query\UK;

class Fitment extends \Ebapi\Query\AbstractFitment
{
  protected function getFldMap() {
    $map = array(
      'Car Make' => 'Make',
      'Cars Type' => 'Type',
    );
    return $map;
  }

  protected function sqlRecsDistinct($fld, $where)
  {
    $fld = $this->mapFldNameFromResp($fld);

    $sql = <<<SQL
SELECT DISTINCT `%s`
FROM {ebapi_uk_fitment}
%s
ORDER BY `%s` ASC
SQL;

    if ($where) {
      $where .= ' AND LENGTH(`' . $fld . '`) > 0';
    }
    else {
      $where = 'WHERE LENGTH(`' . $fld . '`) > 0';
    }

    $sql = sprintf(
      $sql,
      $fld, $where, $fld
    );
    return $sql;
  }

  protected function getWhereForYearRange() {
    list($yfrom, $yto) = $this->yrange;

    $sqlSearchYr = 'SELECT DISTINCT fit_id FROM ebapi_uk_fitment_year WHERE make="%s"';
    $sqlSearchYr = sprintf($sqlSearchYr, $this->cri['Make']);

    if ($yfrom && $yto) {
      $sqlSearchYr .= sprintf(' AND (`year` BETWEEN "%s" AND "%s")', $yfrom, $yto);
    } else if ($yfrom) {
      $sqlSearchYr .= sprintf(' AND `year` >= "%s"', $yfrom);
    } else if ($yto) {
      $sqlSearchYr .= sprintf(' AND `year` <= "%s"', $yto);
    }

    $rs = db_query($sqlSearchYr);
    $ids = array();

    while ($data = db_result($rs)) {
      $ids[] = $data;
    }

    $where = implode(',', $ids);
    $where = 'id IN (' . $where . ')';
    return $where;
  }

  protected function getFitmentMainDisplay() {
    return 'Variant';
  }

  protected function getFitmentSubDisplay() {
    return 'Type';
  }

  protected function getFitmentIdFld() {
    return 'Ktype';
  }

  protected function getIgnoreFldInWhere() {
    if ($_GET['q'] == 'ebapi/fitment/import/js') {
      return array(
        'Cars Year',
//        'Variant'
      );
    }
    else {
      return array(
        'Cars Year',
      );
    }
  }

  protected function sqlGetRecs($flds, $order_by) {
    foreach ($flds as &$fld) {
      $fld = $this->mapFldNameFromResp($fld);
    }

    $sql = sprintf(
      'SELECT %s FROM {ebapi_uk_fitment} %s ORDER BY %s ASC',
      implode($flds, ','), $this->getWhere(), $order_by
    );
    return $sql;
  }

  protected function getListId($rec) {
    $id = sprintf('%s-%s-%s',
      $rec->{$this->getFitmentIdFld()},
      $this->yrange[0],
      $this->yrange[1]
    );
    return $id;
  }
}