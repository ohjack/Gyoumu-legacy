<?php
namespace Oerp\Product\Command;

abstract class Abstract_ProdTplCmd
  extends \Gulei\Command\WithReference\WithTable {

  protected function getTmplRef() {
    list($recs) = $this->ref->vals('recs');
    $tpl_ids = array();

    foreach ($recs as $rec) {
      $id = $rec['product_id.id'];

      if (is_numeric($id)){
        $tpl_ids[] = $id;
      }
    }

    $tpl_ids = array_values($tpl_ids);

    if (!$tpl_ids) {
      return array();
    }

    $prods = \Oerp\Query::q(
      'product.product',
      array(array('product_tmpl_id', 'in', $tpl_ids)),
      array('product_tmpl_id', 'default_code')
    );
    return $prods->promote('product_tmpl_id');
  }
}