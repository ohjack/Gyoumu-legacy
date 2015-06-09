<?php
namespace Oerp\Csv;

class Buffer extends \Gulei\Csv\Buffer
{
  protected function getFuncPrefix()
  {
    return array_merge(array('OerpFunc_'), parent::getFuncPrefix());
  }
}