<?php
namespace Checkimage\Provider;

class Local extends \Checkimage\Provider\_Abstract
{
  const ROOT = '/home/jeremy/fixpic';

  public function getEntryName()
  {
    return 'select-local_paths';
  }

  protected function getEntryTitle()
  {
    return 'Local Paths';
  }

  protected function getPaths() {
    $PathFinder = new \Gulei\File\Finder\Local(self::ROOT . '/publish');
    $path_src = $PathFinder->getList('*', TRUE, GLOB_ONLYDIR);
    $path_src = preg_replace('@' . self::ROOT . '(.*)@', '$1|' . self::ROOT . '$1', $path_src);
    return $this->getSrcPaths($path_src, '(Local)');
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