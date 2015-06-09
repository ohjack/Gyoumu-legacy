<?php
namespace Gulei;

spl_autoload_register(__NAMESPACE__ . '\AutoLoader::load');
Autoloader::register(__DIR__);

class Autoloader
{
  static public function register($dir)
  {
    set_include_path(get_include_path() . PATH_SEPARATOR . $dir);
  }

  static private function hasNamespace($name)
  {
    return strpos($name, "\\") !== false;
  }

  static public function load($className)
  {
    if(self::hasNamespace($className)){
      self::loadNamespace($className);
    }
    else{
      self::loadBasic($className);
    }
  }

  static public function loadNamespace($classname)
  {
    $path_toks = explode("\\", $classname);

    $path = sprintf(
      'inc%s%s.inc',
      DIRECTORY_SEPARATOR,
      implode(DIRECTORY_SEPARATOR, $path_toks)
    );
    self::doInclude($path);
  }

  static public function loadBasic($className)
  {
    $filePath = str_replace('_', DIRECTORY_SEPARATOR, $className) . '.php';
    self::doInclude($filePath);
  }

  static private function doInclude($path)
  {
    $includePaths = explode(PATH_SEPARATOR, get_include_path());
    foreach($includePaths as $includePath){
      $fullpath = $includePath . DIRECTORY_SEPARATOR . $path;

      if(file_exists($fullpath)){
        include_once $fullpath;
        return;
      }
    }
  }
}