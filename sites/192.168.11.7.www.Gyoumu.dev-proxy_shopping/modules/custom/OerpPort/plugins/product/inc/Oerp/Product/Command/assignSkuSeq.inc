<?php
namespace Oerp\Product\Command;

class assignSkuSeq implements \Gulei\Command\_Interface
{
  const OFFSET_VARNAME = 'oerp_product_sku_seq_offset';

  public function execute($refVar = null)
  {
    $q = new \Oerp\Query\Basic(
      'product.product',
      array(array('default_code', 'like', '-%SEQ%')),
      array('id', 'default_code')
    );

    $items = (array)$q->raw(false, true);

    if($items){
      $offset = variable_get(self::OFFSET_VARNAME, null);

      foreach($items as $item){
        $seq = $item['id'] + $offset;
        $qItem = new \Oerp\Query\Basic('product.product', null, null, $item['id']);
        $newSKU = str_replace('%SEQ%', sprintf('%05s', $seq), $item['default_code']);
        $qItem->write(array('default_code' => $newSKU));
      }
    }
  }
}