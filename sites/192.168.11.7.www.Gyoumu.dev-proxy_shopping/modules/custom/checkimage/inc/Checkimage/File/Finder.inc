<?php
namespace Checkimage\File;

class Finder extends \Gulei\File\Finder\Local
{
  const ROOT = '/home/jeremy/fixpic';

  public function __construct($dir)
  {
    parent::__construct(self::ROOT . $dir);
  }
}