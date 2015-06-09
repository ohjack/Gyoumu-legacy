<?php
namespace Gulei\Util;

class Path {
  static public function get($path, $ex = '') {
    preg_match('@.*/(sites/.*)@', $path, $m);
    return $m[1] . $ex;
  }

  static public function add_js($path, $ex = '') {
    drupal_add_js(self::get($path, $ex));
  }

  static public function add_css($path, $ex = '') {
    drupal_add_css(self::get($path, $ex));
  }
}