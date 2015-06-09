<?php
namespace Gulei\Command;

class isDistinct extends WithReference\_Abstract
{
  public function execute($refVar = null)
  {
    list($recs, $rec, $name, $ind) = array(
      $this->ref->val('recs'),
      $this->ref->val('rec'),
      $this->ref->val('name'),
      $this->ref->val('ind'),
    );

    $val = $rec[$name];

    foreach($recs as $i => $r){
      if(empty($r[$name]) || $i == $ind){
        continue;
      }
      if($r[$name] === $val){
        return false;
      }
    }
    return true;
  }
}