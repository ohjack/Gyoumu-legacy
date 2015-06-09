<?php
namespace Oerp\Purchase\Command;

class FindNearestOrder extends \Oerp\Purchase\Command\Abstract_MoveWithOrder {
  public function execute($refVar = null){
    $move = $this->getMove($this->getSku());

    if ($move) {
      $src = sprintf(
        '<a target="_blank" href="?q=oerp/view/form/purchase.order/0/%s"><span class="gu_value">%s</span></a>',
        $move['po_id'], $move['po_name']
      );
      return $src;
    }
    else {
      return 'N/A';
    }
  }
}