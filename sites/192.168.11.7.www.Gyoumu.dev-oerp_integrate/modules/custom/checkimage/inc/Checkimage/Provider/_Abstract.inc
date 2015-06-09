<?php
namespace Checkimage\Provider;

abstract class _Abstract
{
  abstract public function getEntryName();
  abstract protected function getEntryTitle();
  abstract protected function getPaths();
  abstract protected function getPathsSku();
  abstract protected function getChecker($sku, $paths);

  protected $vals;
  protected $sku_list;
  protected $result = array();

  function __construct($state)
  {
    $this->vals = $state['values'][$this->getEntryName()];
    $this->sku_list = $state['values']['sku_list'];
    $this->query();
  }

  protected function query()
  {
    foreach ($this->getPathsSku() as $loc => $files) {
      foreach($this->getSkus() as $sku){
        $chk = $this->getChecker($sku, $files[$sku]);

        $this->result[$loc][$sku]['found'] = $chk->getFound();
        $this->result[$loc][$sku]['missing'] = $chk->getMissing();
      }
    }
  }

  public function getResult()
  {
    return $this->result;
  }

  protected function getSkus()
  {
    $skus = explode("\n", $this->sku_list);
    $skus = array_map('trim', $skus);
    $skus = array_map('strtoupper', $skus);
    $skus = array_filter($skus);

    sort($skus);
    return $skus;
  }

  protected function getSrcPaths($src, $key_prefix)
  {
    $local_paths = explode("\n", $src);
    $paths = array();

    foreach ($local_paths as $local_path) {
      $toks = explode('|', $local_path);

      $key = $key_prefix . ' ' . $toks[0];
      $paths[$key] = trim($toks[1]);
    }

    return $paths;
  }

  public function getFormEntry()
  {
    return array(
      '#title' => $this->getEntryTitle(),
      '#type' => 'checkboxes',
      '#options' => $this->getPaths(),
      '#default_value' => $this->getSelectedOption(),
    );
  }

  protected function getSelectedOption()
  {
    return ($paths = $this->vals) ? array_filter($paths) : array();
  }

  protected function bunchSkuInFilenames($list, $prefix = '')
  {
    $sku_groups = array();

    foreach ($list as $item) {
      preg_match('/^(..-\d{5})-/', $item, $m);

      if ($m[1]) {
        $sku = strtoupper($m[1]);
        $img_path = ($prefix) ? $prefix . '/' . $item : $item;
        $sku_groups[$sku][] = $img_path;
      }
    }
    return $sku_groups;
  }

  protected function bunchSkuInFolder($path)
  {
    $Finder = new \Gulei\File\Finder\Local($path);
    $list = $this->explodeFileList($Finder->getList(null, true));

    return $this->bunchSkuInFilenames($list);
  }

  protected function getSelectedPaths()
  {
    return array_intersect_key(
      $this->getPaths(), $this->getSelectedOption());
  }
}