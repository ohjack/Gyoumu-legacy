<?php
namespace Checkimage\View;

class Result
{
  private $providers;

  function __construct($providers)
  {
    $this->providers = $providers;
  }

  public function theme()
  {
    $hd = array();
    $hd[] = array(
      'data' => 'SKU',
      'class' => 'sep'
    );

    $rows = array();

    foreach($this->providers as $provider){
      $result = $provider->getResult();

      foreach($result as $loc => $skus) {
        $hd[$loc . '_found'] = $loc;
        $hd[$loc . '_missing'] = array('data' => '- missing', 'class' => 'sep');
        $no_missing = true;

        foreach($skus as $sku => $items){
          if(!isset($rows[$sku]['sku'])){
            $rows[$sku]['sku'] = array(
              'data' => $sku . '<br/><span class="show_thumbnails">preview</span>',
              'class' => 'sep'
            );
          }
          $rows[$sku][$loc . '_found'] = $items['found'];
          $rows[$sku][$loc . '_missing'] = array(
            'data' => $items['missing'],
            'class' => 'sep'
          );

          if(!empty($items['missing'])){
            $no_missing = false;
          }
        }

        if($no_missing){
          unset($hd[$loc . '_missing']);

          foreach(array_keys($skus) as $sku){
            unset($rows[$sku][$loc . '_missing']);
          }
        }
      }
    }

    drupal_add_css(drupal_get_path('module', 'checkimage') . '/checkimage.css');
    drupal_add_js(drupal_get_path('module', 'checkimage') . '/checkimage.js');

    $output = '<span class="show_all_thumbnails">Show all previews</span>';
    $output .= theme_table($hd, $rows, array('class' => 'checkimage-result'));
    return $output;
  }
}