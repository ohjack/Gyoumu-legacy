<?php
namespace Gulei\_Array\Command;

class collect extends _Abstract
{
  public function execute($args = null) {
    list($key, $inner_key) = $args;
    $new_array = array();

    foreach ($this->ArryObj as $item) {
      if (is_array($item[$key])) {
        if (!is_null($inner_key)) {
          $new_array[] = $item[$key][$inner_key];
        }
        else {
          foreach ($item[$key] as $each) {
            $new_array[] = $each;
          }
        }
      }
      else {
        $new_array[] = $item[$key];
      }
    }
    return array_values(array_unique($new_array));
  }
}