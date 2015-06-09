<?php
namespace Checkimage\SkuChecker;

class Rakuten extends _Abstract
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