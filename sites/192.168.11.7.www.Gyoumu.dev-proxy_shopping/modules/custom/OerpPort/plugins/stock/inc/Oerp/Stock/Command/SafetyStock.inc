<?php
namespace Oerp\Stock\Command;

class SafetyStock extends \Gulei\Command\WithReference\WithTable
{
  static $sales;

  private function getSale($id)
  {
    if(is_null(self::$sales)){
      list($recs) = $this->ref->vals('recs');

      $ids = \Gulei\_Array::lambda($recs, 'collect', 'id');
      $Prov = new \Oerp\Stock\Provider\SafetyStock($ids, array());
      self::$sales = $Prov->getData();
    }

    return self::$sales[$id]['sum'];
  }

  public function execute($refVar = null){
    list($rec) = $this->ref->vals('rec');
    return $this->getSale($rec['id']);
  }
}