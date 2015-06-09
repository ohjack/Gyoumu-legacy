<?php
namespace Oerp\Modifier;

class Mapping extends \Gulei\Modifier\Mapping
{
  protected function createMapper($rows)
  {
    return new \Oerp\Mapper\Import($rows);
  }
}