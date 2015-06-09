<?php
namespace Gulei\_Array\Command;

class Connect extends _Abstract
{
  public function execute($args = null) {
    list($hds) = $args;

    $mask = array_fill_keys($hds, TRUE);
    $new_array = array();

    foreach ($this->ArryObj as $item) {
      $new_array[] = array_intersect_key($item, $mask);
    }
    return $new_array;
  }
}