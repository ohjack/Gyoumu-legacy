<?php
namespace Checkimage\Provider;

class PictureMaster extends \Checkimage\Provider\_Abstract
{
  const ROOT = './public_html/ProductPics';

  public function getPaths() {
    $Q = new \Checkimage\Query\PictureMaster(self::ROOT);
    $dirs = $Q->getList();

    foreach ($dirs as $ind => &$dir) {
      if($dir == '.' || $dir == '..' || strpos($dir, '_') === false){
        unset($dirs[$ind]);
        continue;
      }
      $dir = $dir . '|' . self::ROOT . '/' . $dir;
    }

    $src = implode("\n", $dirs);
    return $this->getSrcPaths($src, '(FTP)');
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