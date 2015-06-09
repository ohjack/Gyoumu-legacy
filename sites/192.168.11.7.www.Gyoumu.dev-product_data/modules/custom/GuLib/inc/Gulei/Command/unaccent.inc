<?php
namespace Gulei\Command;

class unaccent extends WithReference\_Abstract
{
  function execute($refVar = null)
  {
    list($rec, $name) = $this->ref->vals('rec', 'name');

    $h = new \Gulei\Text\Helper();
    $val = $rec[$name];
    $newVal = $h->asciiize($val);

    return ($val === $newVal) ? false : $newVal;
  }
}