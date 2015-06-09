<?php
namespace Gulei\Provider;

class Compose implements \Gulei\Provider\_Interface
{
  private $recs;
  private $hds;

  function __construct($recs, $hds)
  {
    $this->recs = $recs;
    $this->hds = $hds;
  }

  public function getData()
  {
    $recs = array();
    foreach($this->recs as $refRec){
      if(empty($refRec)){
        $recs[] = array();
        continue;
      }

      foreach(array_keys($this->hds) as $hdName){
        if(isset($refRec[$hdName]))
          $rec[$hdName] = $refRec[$hdName];

        else
          $rec[$hdName] = null;
      }
      $recs[] = $rec;
    }
    return $recs;
  }

  public function getReference()
  {
    return array('_recs' => $this->recs, '_hds' => $this->hds);
  }

  public function count()
  {
    return count($this->recs);
  }
}