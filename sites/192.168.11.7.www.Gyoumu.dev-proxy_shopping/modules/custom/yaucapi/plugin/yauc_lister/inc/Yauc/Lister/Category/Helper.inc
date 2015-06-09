<?php
namespace Yauc\Lister\Category;

class Helper
{
  const KEY = 'id';
  const TABLENAME = 'yaucapi_lister_category';

  const STATUS_VALID = 1;
  const STATUS_BROKEN = 2;

  private function fetchByAnchor($anchor)
  {
    $sql = 'SELECT * FROM {%s} WHERE anchor="%s" LIMIT 1';
    $rs = db_query($sql, self::TABLENAME, $anchor);
    return db_fetch_object($rs);
  }

  public function getLink($anchor)
  {
    $rec = $this->fetchByAnchor(json_encode($anchor));
    return ($rec->status == self::STATUS_VALID) ? $rec->link : false;
  }

  public function updateRecord($name, $link, $anchor)
  {
    $rec = new \stdClass();
    $rec->name = $name;
    $rec->link = $link;
    $rec->anchor = json_encode($anchor);
    $rec->status = self::STATUS_VALID;
    $rec->validate_time = time();

    if($cate = $this->fetchByAnchor($rec->anchor)){
      $rec->id = $cate->id;
      drupal_write_record(self::TABLENAME, $rec, self::KEY);
    }
    else{
      drupal_write_record(self::TABLENAME, $rec);
    }
  }

  public function markAsBroken($anchor)
  {
    $rec = new \stdClass();
    $rec->anchor = json_encode($anchor);
    $rec->validate_time = time();

    if($cate = $this->fetchByAnchor($rec->anchor)){
      $rec->id = $cate->id;
      $rec->status = self::STATUS_BROKEN;
      drupal_write_record(self::TABLENAME, $rec, self::KEY);
    }
  }
}