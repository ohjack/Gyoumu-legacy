<?php
namespace Yauc\Lister\Provider;

class Queue extends _Abstract
{
  public function __construct($sellerId, $status = null)
  {
    $this->sellerId = $sellerId;

    if(!is_null($status) && !is_array($status)){
      $status = array($status);
    }
    $this->status = $status;
  }

  public function count()
  {
    $sql = $this->getSqlCount();
    return db_result(db_query($sql, $this->sellerId));
  }
}