<?php
namespace Gulei\Importer;

abstract class _Abstract {
  abstract public function getArch();
  abstract public function submit($form, &$form_state);
  abstract protected function getClassname();

  static public function createMenuEntry($classname) {
    return array(
      'page callback' => 'drupal_get_form',
      'page arguments' => array('gulib_csv_import_form', $classname),
      'access callback' => TRUE,
      'type' => MENU_CALLBACK,
    );
  }

  public function getMenuEntry() {
    return array(
      'page callback' => 'drupal_get_form',
      'page arguments' => array('gulib_csv_import_form', $this->getClassname()),
      'access callback' => TRUE,
      'type' => MENU_CALLBACK,
    );
  }

  public function getEles($form_state) {
    return array();
  }

  public function theme($form) {
    return drupal_render($form);
  }
}