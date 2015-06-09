<?php
namespace Gulei\Page;

class Helper
{
  static public function createEntry($classname, $param = array())
  {
    return array_merge(
      array(
        'page callback' => 'drupal_get_form',
        'page arguments' => array('gulib_form_form', $classname),
        'access callback' => true,
        'type' => MENU_CALLBACK,
      ),
      $param
    );
  }
}