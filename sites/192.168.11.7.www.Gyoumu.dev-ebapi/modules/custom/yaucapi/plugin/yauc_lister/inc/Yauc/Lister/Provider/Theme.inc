<?php
namespace Yauc\Lister\Provider;

class Theme extends Record
{
  public function getData()
  {
    $val = parent::getData();
    return $val['HTML'];
  }
}