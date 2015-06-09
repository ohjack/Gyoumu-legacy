<?php
namespace Gulei\Exception;

class _Throw
{
  static public function msg($msg)
  {
    echo '<pre>';
    throw new \Exception($msg);
  }
}