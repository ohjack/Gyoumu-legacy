<?php
namespace Gulei\Modifier\Pack;

abstract class _Abstract
{
  protected $items = array();

  public function __construct()
  {
    $this->init();
  }

  abstract protected function init();

  protected function append(array $mods)
  {
    $this->items = array_merge($this->items, $mods);
  }

  public function getItems()
  {
    return $this->items;
  }
}