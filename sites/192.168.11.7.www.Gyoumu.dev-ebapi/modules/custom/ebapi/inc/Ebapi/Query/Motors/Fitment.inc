<?php
namespace Ebapi\Query\Motors;

class Fitment extends \Ebapi\Query\AbstractFitment
{
  protected function sqlRecsDistinct($fld, $where)
  {
    return sprintf(
      'SELECT DISTINCT `%s` FROM {ebapi_fitment} %s ORDER BY `%s` ASC',
      $fld, $where, $fld
    );
  }

  protected function getWhereForYearRange() {
    list($yfrom, $yto) = $this->yrange;

    if ($yfrom && $yto) {
      return sprintf('(`Year` BETWEEN "%s" AND "%s")', $yfrom, $yto);
    } else if ($yfrom) {
      return sprintf('`Year` >= "%s"', $yfrom);
    } else if ($yto) {
      return sprintf('`Year` <= "%s"', $yto);
    }
    return false;
  }

  protected function getFitmentMainDisplay() {
    return 'DisplayName';
  }

  protected function getFitmentSubDisplay() {
    return 'Body';
  }

  protected function getFitmentIdFld() {
    return 'ePID';
  }

  protected function sqlGetRecs($flds, $order_by) {
    $sql = sprintf(
      'SELECT %s FROM {ebapi_fitment} %s ORDER BY %s ASC',
      implode($flds, ','), $this->getWhere(), $order_by
    );
    return $sql;
  }
}