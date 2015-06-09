<?php
namespace Checkimage\SkuChecker;

abstract class _Abstract
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