<?php
namespace Oerp\Gallery\File\Editor;

class toPhoto
{
  protected $root;

  public function __construct()
  {
    $this->root = drupal_get_path('module', 'oerp_gallery') . '/gallery';
  }

  public function append($toPhotos)
  {
    $fp = fopen($this->root . '/toPhoto', 'a');
    fputs($fp, implode("\n", $toPhotos) . "\n");
    fclose($fp);
  }
}