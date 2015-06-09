<?php
namespace Oerp\Delivery\Command;

class Get4pxItemCode extends \Gulei\Command\WithReference\WithTable {
  static protected $code_map = NULL;

  public function execute($ref_val = null) {
    if (self::$code_map === NULL) {
      $fpath = '/home/jeremy/DiskP/4PX/4px_item_codes.csv';

      if (file_exists($fpath)) {
        $Csv = new \Gulei\Csv\Model(array('filepath' => $fpath));
        self::$code_map = \Gulei\_Array::lambda($Csv->connect(), 'promote', 'SKU');
      }
      else {
        self::$code_map = array();
      }
    }

    $rec = $this->ref->vals('rec');
    $rec = $rec[0];

    $sku = $rec['move_lines.product_id.default_code'];
    $carr = $rec['carrier_id'];

    if (stripos($carr, '4px-') === 0) {
      return self::$code_map[$sku]['code'];
    }
    return FALSE;
  }
}