<?php
namespace Oerp\Includer;

class View implements \Gulei\Includer\_Interface
{
  private function includeDepCkeditor()
  {
    drupal_add_js(\libraries_get_path('ckeditor') . '/ckeditor.js');
    drupal_add_js(\libraries_get_path('ckeditor') . '/adapters/jquery.js');
    drupal_add_js(\libraries_get_path('ckeditor') . '/jquery_dialog_fix.js');
  }

  public function includeCss()
  {
    drupal_add_css(drupal_get_path('module', 'GuLib') . '/css/table.css');
    \Gulei\Form\Basic::addCss();
    \Oerp\Util\Helper::addCss();
  }

  public function includeJs()
  {
    drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/GuTable.js');
    \Gulei\Form\Basic::addJs();
    \Oerp\Util\Helper::addJs('jquery.oerpview', 'formview', 'json2');
    $this->includeDepCkeditor();
  }
}