<?php
namespace Oerp\Product\Command;

class GetSkuFromProdTpl extends \Oerp\Product\Command\Abstract_ProdTplCmd {
  public function execute($refVar = null) {
    static $skus = NULL;

    if ($skus === NULL) {
      $skus = $this->getTmplRef();
    }

    list($rec) = $this->ref->vals('rec');
    return $skus[$rec['product_id.id']]['default_code'];
  }
}