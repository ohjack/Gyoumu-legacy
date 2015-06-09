<?php
namespace Yauc\Lister\Provider;

class Record extends _Abstract
{
  public function __construct($rid)
  {
    $this->id = $rid;
  }

  public function count()
  {
    return 1;
  }

  public function getData()
  {
    $rec = parent::getData();
    $rec = $rec[0];
    unset($rec['category']);
    return array_map('trim', $rec);
  }
}