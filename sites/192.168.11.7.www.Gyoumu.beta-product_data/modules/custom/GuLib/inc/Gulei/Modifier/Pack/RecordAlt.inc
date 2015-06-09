<?php
namespace Gulei\Modifier\Pack;
use Gulei\Modifier as M;

class RecordAlt extends _Abstract
{
  protected function init()
  {
    $this->append(
      array(
        new M\CopyMerge(),
        new M\Nl2br(),
        new M\CopyDuplicate(),
        new M\RecordProcessor\Translate(),
        new M\RecordProcessor\Filter(),
        new M\Usort(),
        new M\AddCount(),
        new M\RecordProcessor\Validate(),
        new M\EscapeXml(),
      )
    );
  }
}