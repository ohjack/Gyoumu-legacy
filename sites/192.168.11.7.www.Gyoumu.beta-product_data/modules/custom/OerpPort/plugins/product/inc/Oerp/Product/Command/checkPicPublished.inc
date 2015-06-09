<?php
namespace Oerp\Product\Command;

class checkPicPublished extends Abstract_checkPic
{
  protected function getCacheId()
  {
    return __CLASS__;
  }

  protected function createFinder()
  {
    return new \Checkimage\Query\PictureMaster('./public_html/ProductPics/titosoy');
  }
}