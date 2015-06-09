<?php
namespace Oerp\Gallery\File\Finder;

class Gallery extends \Gulei\File\Finder\Local
{
  public function __construct($dir)
  {
    $root = drupal_get_path('module', 'oerp_gallery') . '/gallery';
    parent::__construct($root . '/' . $dir);
  }
}