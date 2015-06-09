<?php
namespace Ebapi\Page\MySelling;

class ListingTable extends \Gulei\Table\Select
{
  protected function getSelectionValue($rec, $ind){
    return $rec['ItemID'];
  }
}