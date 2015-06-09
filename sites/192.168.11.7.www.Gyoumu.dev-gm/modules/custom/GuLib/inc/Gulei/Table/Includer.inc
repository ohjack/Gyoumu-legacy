<?php
namespace Gulei\Table;

class Includer implements \Gulei\Includer\_Interface
{
  function includeCss()
  {
    drupal_add_css(drupal_get_path('module', 'GuLib') . '/css/table.css');
  }

  function includeJs()
  {
    drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/GuTable.js');
  }
}