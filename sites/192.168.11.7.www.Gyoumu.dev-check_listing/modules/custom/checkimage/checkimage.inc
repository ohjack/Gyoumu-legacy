<?php

require_once(dirname(__FILE__) . '/lib/FtpChecker/FtpChecker.inc');

class CheckImageController
{
  private $state;
  private $providers;
  
  function __construct($state)
  {
    $this->state = $state;
    $this->providers[] = new ProviderCheckimageLocal($state);
    $this->providers[] = new ProviderCheckimagePm($state);
    $this->providers[] = new ProviderCheckimageRt($state);
  }
  
  private function getValues($name = null)
  {
    if (is_null($name))
      return $this->state['values'];

    return $this->state['values'][$name];
  }

  private function hasValues()
  {
    return isset($this->state['values']);
  }

  private function getPathSelectionEntry()
  {
    $form = array();

    foreach($this->providers as $p){
      $form[$p->getEntryName()] = $p->getFormEntry($this->state);
    }

    return $form;
  }

  private function getOptionEntry()
  {
    $opts = array();
    $opts['paths'] = $this->getPathSelectionEntry();

    $opts['sku_list'] = array(
      '#title' => 'SKU List',
      '#type' => 'textarea',
      '#description' => 'Enter SKUs by lines.',
      '#default_value' => $this->getSkuList(),
    );

    $opts['load'] = array(
      '#title' => 'Load from database',
      '#type' => 'checkbox',
    );

    $opts['check'] = array(
      '#type' => 'submit',
      '#value' => 'Check',
    );

    $form = array(
      '#title' => 'Check Options',
      '#type' => 'fieldset',
      '#collapsible' => true,
      $opts
    );

    return $form;
  }

  private function getSkuList()
  {
    return $this->getValues('sku_list');
  }

  private function getSelectedOption()
  {
    $paths = array();
    foreach($this->providers as $Provider){
      $paths[$Provider->getEntryName()] = $Provider->getSelectedOption();
    }

    return $paths;
  }

  public function getFormEntry()
  {
    $form = array();
    $form['option'] = $this->getOptionEntry();

    if ($this->hasValues()) {
      $tb = new TableCheckimage(
        $this->providers
      );

      $form['result'] = array(
        '#type' => 'markup',
        '#value' => $tb->theme()
      );
    }
    return $form;
  }
}

abstract class DataProviderCheckimageAbstract
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
    $Finder = new GuFileFinder($path);
    $list = $this->explodeFileList($Finder->getList(null, true));

    return $this->bunchSkuInFilenames($list);
  }

  protected function getSelectedPaths()
  {
    return array_intersect_key(
      $this->getPaths(), $this->getSelectedOption());
  }
}

class ProviderCheckimageLocal extends DataProviderCheckimageAbstract
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
    return new CheckerSkuLocal($sku, $paths);
  }
}

class ProviderCheckimagePm extends DataProviderCheckimageAbstract
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
      $PmFetcher = new FetcherPictureMaster($path);
      $path = $this->bunchSkuInFilenames($PmFetcher->getList(), $path);
    }
    return $paths;
  }

  protected function getChecker($sku, $paths)
  {
    return new CheckerSkuFtp($sku, $paths);
  }
}

class ProviderCheckimageRt extends DataProviderCheckimageAbstract
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
    return new CheckerSkuRt($sku, $paths);
  }
}

class TableCheckimage
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

abstract class CheckerSkuAbstract
{
  protected $sku;
  protected $paths;

  abstract public function getFound();

  function __construct($sku, $paths)
  {
    $this->sku = $sku;
    $this->paths = $paths;
  }

  private function getMissingFiles()
  {
    $sku = $this->sku;
    $list = $this->paths;

    if (is_array($list)) {
      $list = array_map('strtolower', $list);
      $list = array_map('basename', $list);
      $missing = array();

      for ($i = 1; $i <= 3; $i++) {
        $fname = $sku . "-" . $i . ".jpg";

        if (!in_array(strtolower($fname), $list, true)) {
          $missing[] = $fname;
        }
      }
      return $missing;
    }
    else {
      $expected = array();
      for ($i = 1; $i <= 3; $i++) {
        $expected[] = $sku . "-" . $i . ".jpg";
      }
      return $expected;
    }
  }

  function getMissing()
  {
    return implode('<br>', $this->getMissingFiles());
  }
}

class CheckerSkuLocal extends CheckerSkuAbstract
{
  function getFound()
  {
    return is_array($this->paths)
      ? implode('<br/>', $this->paths) : '';
  }
}

class CheckerSkuFtp extends CheckerSkuAbstract
{
  public function getFound()
  {
    $paths = $this->paths;

    if(!is_array($paths)){
      return '';
    }

    foreach($paths as &$path){
      preg_match('@public_html/(.*)@', $path, $m);

      $path = sprintf(
        '<a target="_blank" href="http://%s/%s">%s</a>',
        FetcherPictureMaster::host, $m[1], basename($path)
      );
    }
    return implode('<br />', $paths);
  }
}

class CheckerSkuRt extends CheckerSkuAbstract
{
  public function getFound()
  {
    $paths = $this->paths;
    foreach($paths as $num => &$path){
      $path = sprintf(
        '<img alt="Missing %s" src="%s" style="width:250px;"/>',
        $path, $path
      );
    }
    return implode('<br />', $paths);
  }
}

class FetcherPictureMaster
{
  const host = 'picturemaster.info';
  static protected $checker;
  private $path;

  function __construct($path)
  {
    if (!self::$checker) {
      self::$checker = new FtpChecker(self::host, 'pictu39', '2xaugadr');
      self::$checker->login();
    }

    $this->path = $path;
  }

  public function getList()
  {
    return self::$checker->getList($this->path);
  }
}


?>