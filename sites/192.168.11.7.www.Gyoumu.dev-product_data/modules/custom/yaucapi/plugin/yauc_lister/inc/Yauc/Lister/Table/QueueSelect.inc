<?php
namespace Yauc\Lister\Table;

namespace Yauc\Lister\Table;
class QueueSelect extends \Gulei\Table\Select
{
  protected function getSelectionValue($rec, $ind)
  {
    return $rec['id'];
  }
}