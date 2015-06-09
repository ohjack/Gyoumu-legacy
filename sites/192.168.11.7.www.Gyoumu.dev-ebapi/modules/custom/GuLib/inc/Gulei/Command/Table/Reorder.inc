<?php
namespace Gulei\Command\Table;

class Reorder extends _Abstract
{
  public function execute($refVar = null)
  {
    $func = $this->ref->val('func');
    $recs = &$refVar->refVar('recs');

    foreach($func['src'] as $src){
      $funcSort = create_function('$rec1, $rec2', $src->nodeValue);
      usort($recs, $funcSort);
    }

    if (!$recs[0]['id']) {
      array_push($recs, array_shift($recs));
    }
  }
}