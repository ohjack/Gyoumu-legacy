<?php
namespace Yauc\Query;

class Item extends _Abstract
{
  public function query($sku)
  {
    $alive = array(self::AUC_NEW, self::AUC_LOADED);

    $sql = sprintf('SELECT * FROM {%s} WHERE sku=UCASE("%s") AND status IN (%s)',
      self::table_name, strtoupper($sku), implode(',', $alive)
    );

    $rs = db_query($sql);
    $items = array();

    while($data = db_fetch_object($rs)){
      $items[] = $data;
    }
    return $items;
  }
}