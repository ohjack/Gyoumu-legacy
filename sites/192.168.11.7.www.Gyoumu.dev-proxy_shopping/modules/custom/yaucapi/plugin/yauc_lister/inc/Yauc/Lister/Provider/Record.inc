<?php
namespace Yauc\Lister\Provider;

class Record extends Abstract_Themed
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

    foreach($rec as &$fld){
      if(!is_array($fld)){
        $fld = trim($fld);
      }
    }
    return $rec;
  }
}