<?php
namespace Yauc\Lister\Provider;

class Theme extends Record
{
  public function getData()
  {
    $val = parent::getData();
    $rec = $this->applyTemplate($val);
    return $rec['HTML'];
  }
}