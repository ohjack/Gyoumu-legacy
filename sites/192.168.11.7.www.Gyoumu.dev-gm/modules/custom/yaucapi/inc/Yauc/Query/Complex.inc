<?php
namespace Yauc\Query;

class Complex extends _Abstract
{
  protected $sql_select = array();
  protected $sql_wheres = array();

  public function __construct($select)
  {
    $this->sql_select = $select;
  }

  public function addFilter($filter)
  {
    if($sql_where = $filter->getSqlWhere()){
      $this->sql_wheres[] = $sql_where;
    }
  }

  protected function getSqlFrom()
  {
    return self::table_name;
  }

  protected function getSqlWhere()
  {
    $cri = array();
    foreach($this->sql_wheres as $cro){
      $cri[] = '(' . $cro . ')';
    }
    return implode(' AND ', $cri);
  }

  public function query()
  {
    $sql = sprintf(
      'SELECT %s FROM %s WHERE %s',
      $this->sql_select, $this->getSqlFrom(), $this->getSqlWhere()
    );
    print_r($sql);
    $rs = db_query($sql);
    $recs = array();

    while($data = db_fetch_array($rs)){
      $recs[] = $data;
    }
    return $recs;
  }
}