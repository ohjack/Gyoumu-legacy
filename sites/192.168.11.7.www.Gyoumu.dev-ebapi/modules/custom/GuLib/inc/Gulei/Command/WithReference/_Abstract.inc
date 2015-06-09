<?php
namespace Gulei\Command\WithReference;

abstract class _Abstract extends \Gulei\Reference\Wrapper\_Abstract implements \Gulei\Command\_Interface {
  protected function getRootName() {
    list($name) = $this->ref->vals('name');

    $name_toks = explode('.', $name);
    array_pop($name_toks);

    $root = implode('.', $name_toks);
    $root .= $root ? '.' : '';
    return $root;
  }

  protected function getLeafName() {
    list($name) = $this->ref->vals('name');
    $name_toks = explode('.', $name);
    return array_pop($name_toks);
  }
}