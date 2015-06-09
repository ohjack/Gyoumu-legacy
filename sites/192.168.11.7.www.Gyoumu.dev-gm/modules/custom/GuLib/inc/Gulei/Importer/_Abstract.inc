<?php
namespace Gulei\Importer;

abstract class _Abstract
{
  abstract public function getArch();
  abstract public function submit($form, &$form_state);
  abstract protected function getClassname();

  public function getMenuEntry()
  {
    return array(
      'page callback' => 'drupal_get_form',
      'page arguments' => array('gulib_csv_import_form', $this->getClassname()),
      'access callback' => true,
      'type' => MENU_CALLBACK,
    );
  }
}