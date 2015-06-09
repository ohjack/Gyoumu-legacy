<?php
namespace Checkimage\Provider;

class Local extends _Abstract
{
  private $root = '/home/jeremy/SmbGallery';

  public function getEntryName()
  {
    return 'select-local_paths';
  }

  protected function getEntryTitle()
  {
    return 'Local Paths';
  }

  protected function getPaths()
  {
    return $this->getSrcPaths(variable_get('checkimage:local_paths', ''), '(Local)');
  }

  protected function explodeFileList($list)
  {
    $list = str_replace('\\', '/', $list);
    $list = explode("\n", $list);

    if (count($list) == 1) {
      $list = $list[0];
      $list = explode(";", $list);
    }

    foreach ($list as &$item) {
      $item = basename($item);
    }

    return $list;
  }

  protected function getPathsSku()
  {
    $paths = $this->getSelectedPaths();

    foreach ($paths as &$path) {
      $path = $this->bunchSkuInFolder($this->root . $path);
    }
    return $paths;
  }

  protected function getChecker($sku, $paths)
  {
    return new \Checkimage\SkuChecker\Local($sku, $paths);
  }
}