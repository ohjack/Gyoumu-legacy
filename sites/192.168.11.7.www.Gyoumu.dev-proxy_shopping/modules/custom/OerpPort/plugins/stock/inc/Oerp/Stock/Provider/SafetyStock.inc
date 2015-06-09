<?php
namespace Oerp\Stock\Provider;

class SafetyStock extends \Gulei\Provider\_Abstract
{
  private $ids;
  private $cri;
  private $periods = 4;

  public function __construct($ids, $cri = array())
  {
    $this->ids = $ids;
    $this->cri = $cri;
  }

  protected function stdev($aValues, $bSample = false)
  {
    $fMean = array_sum($aValues) / count($aValues);
    $fVariance = 0.0;
    foreach ($aValues as $i)
    {
      $fVariance += pow($i - $fMean, 2);
    }
    $fVariance /= ( $bSample ? count($aValues) - 1 : count($aValues) );
    return (float) sqrt($fVariance);
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
      $vals = $sale['period'];
      $avg = array_sum($vals) / count($vals);
      $sale['sum'] = round($avg * 2 + $this->stdev($sale['period']));
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