<?php
namespace Gulei\Includer\JS;

class Datepicker {
  static public function inc() {
    jquery_ui_add('ui.datepicker');
    drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/jquery-ui-timepicker-addon.js');
    \Gulei\Util\Path::add_js(__DIR__ . '/Datepicker.js');
  }
}