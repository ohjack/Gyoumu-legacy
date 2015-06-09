<?php
namespace Checkimage\SkuChecker;

class Ftp extends _Abstract
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
        \Checkimage\Query\PictureMaster::host, $m[1], basename($path)
      );
    }
    return implode('<br />', $paths);
  }
}