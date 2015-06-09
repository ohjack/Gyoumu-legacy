<?php
namespace Gulei\File;

class Helper
{
  static public function getPathContent($type, $name, $filepath){
    $path = drupal_get_path($type, $name) . $filepath;
    return file_get_contents($path);
  }
}