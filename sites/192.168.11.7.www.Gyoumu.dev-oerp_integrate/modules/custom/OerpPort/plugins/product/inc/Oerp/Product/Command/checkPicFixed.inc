<?php
namespace Oerp\Product\Command;

class checkPicFixed extends Abstract_checkPic
{
  protected function getCacheId()
  {
    return __CLASS__;
  }

  protected function createFinder()
  {
    return new \Checkimage\File\Finder('/Fixed');
  }
}