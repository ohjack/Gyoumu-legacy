<?php
namespace Checkimage\SkuChecker;

class Local extends _Abstract
{
  function getFound()
  {
    return is_array($this->paths)
      ? implode('<br/>', $this->paths) : '';
  }
}