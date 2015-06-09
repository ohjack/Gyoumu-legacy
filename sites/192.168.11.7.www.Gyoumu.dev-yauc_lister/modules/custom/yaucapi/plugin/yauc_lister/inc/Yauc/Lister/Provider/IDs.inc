<?php
namespace Yauc\Lister\Provider;

class IDs extends Queue
{
  protected function getSqlSelect()
  {
    return 'id';
  }
}