<?php
namespace Oerp\Product\Command;

class GetStockFromProdTpl extends \Oerp\Product\Command\Abstract_ProdTplCmd {
  public function execute($refVar = null) {
    static $ref = NULL;
    static $stock = NULL;

    if ($ref === NULL) {
      $ref = $this->getTmplRef();

      if (!$ref) {
        $stock = array();
      }
      else {
        $pids = $ref->collect('id');
        $provStock = new \Oerp\Stock\Provider\SafetyStock($pids);

        $stock = \Oerp\Query::q(
          'product.product',
          NULL,
          array(
            'qty_available', 'virtual_available'
          ),
          $pids
        );

        $stock = $stock->promote('product_tmpl_id');

        foreach ($provStock->getData() as $stock_line) {
          $tpl_id = $stock_line['product_tmpl_id'][0];
          $stock[$tpl_id]['safety_stock'] = $stock_line['sum'];
        }
      }
    }

    list($rec, $name) = $this->ref->vals('rec', 'name');
    $tpl_id = $rec['product_id.id'];

    switch ($name) {
      case 'func:real_stock':
        return $stock[$tpl_id]['qty_available'];

      case 'func:virt_stock':
        return $stock[$tpl_id]['virtual_available'];

      case 'func:safe_stock':
        return $stock[$tpl_id]['safety_stock'];
    }
  }
}