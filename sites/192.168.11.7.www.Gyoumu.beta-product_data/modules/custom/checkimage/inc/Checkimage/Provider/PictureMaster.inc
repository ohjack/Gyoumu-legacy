<?php
namespace Checkimage\Provider;

class PictureMaster extends _Abstract
{
  public function getPaths()
  {
    return $this->getSrcPaths(variable_get('checkimage:pm_paths', ''), '(FTP)');
  }

  public function getEntryName()
  {
    return 'select-pm_paths';
  }

  protected function getEntryTitle()
  {
    return 'Paths on picturemaster.info';
  }

  protected function getPathsSku()
  {
    $paths = $this->getSelectedPaths();

    foreach ($paths as &$path) {
      $PmFetcher = new \Checkimage\Query\PictureMaster($path);
      $path = $this->bunchSkuInFilenames($PmFetcher->getList(), $path);
    }
    return $paths;
  }

  protected function getChecker($sku, $paths)
  {
    return new \Checkimage\SkuChecker\Ftp($sku, $paths);
  }
}