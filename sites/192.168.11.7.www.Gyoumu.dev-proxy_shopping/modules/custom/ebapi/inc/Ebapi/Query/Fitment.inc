<?php
namespace Ebapi\Query;

class Fitment
{
  private  $cri;
  private $yrange;

  public function __construct($cri, $yrange)
  {
    $this->cri = $cri;
    $this->yrange = $yrange;
  }

  public function getRecs($flds, $order_by)
  {
    $sql = sprintf(
      'SELECT %s FROM {ebapi_fitment} %s ORDER BY %s ASC',
      implode($flds, ','), $this->getWhere(), $order_by
    );

    $rs = db_query($sql);
    $recs = array();

    while($data = db_fetch_object($rs)){
      $recs[] = $data;
    }
    return $recs;
  }

  public function getFitmentList()
  {
    $recs = $this->getRecs(
      array('ePID', 'DisplayName', 'Body'), 'DisplayName');

    $list = array();
    foreach($recs as $rec){
      $list[$rec->ePID] = $rec->DisplayName . ' ' . $rec->Body;
    }
    return $list;
  }

  private function getRecsDistinct($fld)
  {
    $sql = sprintf(
      'SELECT DISTINCT `%s` FROM {ebapi_fitment} %s ORDER BY `%s` ASC',
      $fld, $this->getWhere(), $fld
    );

    $rs = db_query($sql);
    $recs = array();

    while($data = db_result($rs)){
      $recs[] = $data;
    }
    return $recs;
  }

  public function getSelectOptions($name)
  {
    $opts = array('0' => '-SELECT-');
    $recs = $this->getRecsDistinct($name, $name);

    foreach($recs as $rec){
      $opts[$rec] = $rec;
    }
    return $opts;
  }

  public function getWhere()
  {
    $where = array();
    foreach($this->cri as $key => $val){
      if($val){
        $where[] = sprintf('`%s` = "%s"', $key, $val);
      }
    }

    if($this->yrange){
      list($yfrom, $yto) = $this->yrange;

      if($yfrom && $yto){
        $where[] = sprintf('(`Year` BETWEEN "%s" AND "%s")', $yfrom, $yto);
      }
      else if($yfrom){
        $where[] = sprintf('`Year` >= "%s"', $yfrom);
      }
      else if($yto){
        $where[] = sprintf('`Year` <= "%s"', $yto);
      }
    }

    if(empty($where)){
      $where = '';
    }
    else{
      $where = 'WHERE ' . implode($where, ' AND ');
    }
    return $where;
  }
}