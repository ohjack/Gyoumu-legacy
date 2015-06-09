<?php
namespace Checkimage\Provider;

class Rakuten extends _Abstract
{
  public function getPaths()
  {
    return $this->getSrcPaths(variable_get('checkimage:rt_paths', ''), '(Rakuten)');
  }

  public function getEntryName()
  {
    return 'select-rt_paths';
  }

  protected function getEntryTitle()
  {
    return 'Paths on Rakuten';
  }

  public function getPathsSku()
  {
    $paths = array();
    foreach($this->getSelectedPaths() as $loc => $path){
      $skus = array_values($this->getSkus());

      if(count($skus) > 30){
        drupal_set_message('Check on Rakuten gallery is limited to 30 SKUs.', 'error');
      }

      foreach($skus as $num => $sku){
        if($num > 30){
          $paths[$loc][$sku] = array();
          continue;
        }

        for($c = 1; $c <=3; $c++){
          $paths[$loc][$sku][] =
              sprintf('%s/items/%s-%s.jpg', $path, strtolower($sku), $c);
        }

        for($c = 1; $c <=3; $c++){
          preg_match('/..-([\d]{5})/', $sku, $m);
          $item_path = 'items' . ceil($m[1] / 500) * 500;

          $paths[$loc][$sku][] =
              sprintf('%s/%s/%s-%s.jpg', $path, $item_path, strtolower($sku), $c);
        }
      }
    }
    return $paths;
  }

  protected function getChecker($sku, $paths)
  {
    return new \Checkimage\SkuChecker\Rakuten($sku, $paths);
  }
}