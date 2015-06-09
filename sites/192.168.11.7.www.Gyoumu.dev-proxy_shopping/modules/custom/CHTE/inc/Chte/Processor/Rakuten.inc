<?php
namespace Chte\Processor;

class Rakuten extends \Chte\Processor\Abstract_PC_MB {
  protected function getPcField() {
    return 'PC用商品説明文';
  }

  protected function getMbField() {
    return 'モバイル用商品説明文';
  }

  protected function getPicUrl($row) {
    $sku = $row['商品番号'];
    $sku_token = explode('-', $sku);
    $ceil = ((int)$sku_token[1] - 1) / 500;
    $ceil = ((int)$ceil + 1) * 500;

    $links = array();
    for($c = 1; $c <= 3; $c++){
      $links[] = sprintf(
        "http://image.rakuten.co.jp/titosoy/cabinet/items%s/%s-%s.jpg",
        $ceil, strtolower($sku), $c
      );
    }
    return implode(" ", $links);
  }

  protected function prepareRecords() {
    parent::prepareRecords();
    foreach ($this->input as &$rec) {
      $rec['商品画像URL'] = $this->getPicUrl($rec);
    }
  }
}