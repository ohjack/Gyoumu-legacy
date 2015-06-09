<?php
namespace Gulei\Link;

class Page
{
  static private function internal($text, $path, $query = array())
  {
    return l($text, $path, array('query' => $query));
  }

  static public function create($text, $classname, $query = array())
  {
    $query['n'] = $classname;
    return self::internal($text, 'gulib/form', $query);
  }
}