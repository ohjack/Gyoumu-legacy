<?php
namespace Oerp\Stock\Importer;

class PeriodicalInventory extends \Gulei\Importer\Arch\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function getArch()
  {
    $fp = __DIR__ . '/PeriodicalInventory.xml';
    return file_get_contents($fp);
  }

  private function getProdRef($recs)
  {
    $skus = (array)\Gulei\_Array::lambda($recs, 'collect', 'SKU');
    $skus = array_map('trim', $skus);

    $q = new \Oerp\Query\Basic(
      'product.product', array(array('default_code', 'in', $skus)), array('id', 'uom_id', 'default_code'));

    return $q->raw()->promote('default_code');
  }

  private function send($send){
    $q = new \Oerp\Query\Basic('stock.inventory');
    return $q->create($send);
  }

  public function submit($form, &$form_state)
  {
    $recs = $this->getRecs($form);
    $ref = $this->getProdRef($recs);

    $date = date('Y-m-d', mktime());


    $recs_batch = array_chunk($recs, 500);
    unset($recs);

    foreach($recs_batch as $ind => $recs){
      $send = array(
        'move_ids' => array(array(6, 0, array())),
        'name' => 'Import:' . $date . '-' . $ind,
        'inventory_line_id'=> array(),
      );

      foreach($recs as $rec){
        $prod = $ref[$rec['SKU']];

        $send['inventory_line_id'][] = array(
          0, 0, array(
            'location_id' => (int)11,
            'product_id' => (int)$prod['id'],
            'product_uom' => (int)$prod['uom_id'][0],
            'product_qty' => (float)$rec['stock'],
          )
        );
      }
      $this->send($send);
    }
  }
}