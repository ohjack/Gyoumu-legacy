<?php
namespace Gulei\Modifier\Pack;
use Gulei\Modifier as M;

class RowRaw extends _Abstract
{
  protected function init()
  {
    $this->append(
      array(
           new M\Tagize(),
           new M\Tag\Hide(),
           new M\Tag\AppendEmpty(),
           new M\Tag\CopyDitto(),
      )
    );
  }
}