<?php
namespace Gulei\Table;

class Arch extends _Abstract
{
  function __construct($arch_src, \Gulei\Provider\_Interface $provRecs, $param = array()){
    parent::__construct(
      $arch_src,
      new \Gulei\Provider\Arch($arch_src),
      $provRecs,
      $param
    );
  }

  protected function getDefaultParam(){
    return array(
      'mark_translated' => true,
      'mark_invalid' => true,
      'mark_notice' => true,
      'mark_missing' => true,
      'copy_method' => 'ditto',
      'nl2br' => true,
      'sticky_header' => true,
    );
  }

  protected function getFuncPrefix()
  {
    return array('GuFunc_');
  }

  protected function includeCssJs()
  {
    drupal_add_css(drupal_get_path('module', 'GuLib') . '/css/table.css');
    drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/GuTable.js');
  }
}