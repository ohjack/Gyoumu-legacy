<?php
namespace Oerp\Stock\Provider;

class SafetyStock implements \Gulei\Provider\_Interface
{
  private $ids;
  private $cri;
  private $periods = 4;

  public function __construct($ids, $cri = array())
  {
    $this->ids = $ids;
    $this->cri = $cri;
  }

  public function getData()
  {
    $sales = array();
    for($offset = 1; $offset <= $this->periods; $offset++){
      $prov = new \Oerp\Stock\Provider\RecentDelivered($this->ids, $this->cri, $offset);

      foreach($prov->getData() as $rec){
        $rid = $rec['id'];
        $qty = $rec['qty_available'];

        if(!isset($sales[$rid])){
          $sales[$rid] = $rec;
          $sales[$rid]['period'] = array($qty);
        }
        else{
          $sales[$rid]['period'][] = $qty;
        }
      }
    }

    foreach($sales as &$sale){
      $sale['sum'] = array_sum($sale['period']);
    }
    return $sales;
  }

  public function count()
  {
    return count($this->ids);
  }

  public function getReference()
  {
    return array();
  }
}