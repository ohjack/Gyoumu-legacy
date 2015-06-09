<?php
namespace Ebapi\Fitment\Motors;

class Helper extends \Ebapi\Fitment\AbstractHelper
{
  public function queryById($ePID)
  {
    $sql = 'SELECT * FROM {ebapi_fitment} WHERE `ePID`="%s" LIMIT 1';
    $rs = db_query($sql, $ePID);
    return db_fetch_object($rs);
  }

  protected function getQuery($cri) {
    return new \Ebapi\Query\Motors\Fitment($cri, null);
  }

  public function expendFitment($id, $rec) {
    return $rec;
  }
}