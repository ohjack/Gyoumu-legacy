<?php
namespace Oerp\Purchase\Hook\JS;

class OnChange_AllowZeroQty extends \Oerp\Hook\JS\OnChange{
  protected $post;

  public function getWeight() {
    return $this->succeedWeight(parent::getWeight());
  }

  public function applicable() {
    if (!parent::applicable()) {
      return FALSE;
    }

    $send = $this->send;

    if ($send[0] == 'purchase.order.line'
        && $send[1] == 'product_id_change') {

      return TRUE;
    }
    return FALSE;
  }

  public function exe() {
    $send = $this->send;
    $qty = $send[5];

    if ($qty == 0) {
      $send = array(
        'value' => array(
          'productg_qty' => 0,
          'price_unit' => 0,
        )
      );
      return drupal_json($send);
    }
    else {
      return parent::exe();
    }
  }
}