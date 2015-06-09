<?php
namespace Oerp\Importer;

class ProductData extends \Gulei\Importer\Arch\_Abstract
{
  private $ref;

  protected function getClassname()
  {
    return __CLASS__;
  }

  public function getArch()
  {
    return file_get_contents(__DIR__ . '/ProductData/arch.xml');
  }

  private function createMissingRef($rec)
  {
    $ids = $this->getRecIdPack($rec);

    if(!$ids['pid']){
      $q = new \Oerp\Query\Basic('product.product');
      $pid = $q->create(
        array(
          'name' => $rec['Name'],
          'default_code' => $rec['SKU'],
        )
      );
      $this->ref['product.product'][$rec['SKU']] = $pid;
      unset($q);
    }

    if(!$ids['sup_id']){
      $q = new \Oerp\Query\Basic('res.partner');
      $sup_id = $q->create(
        array(
          'name' => $rec['Supplier'],
          'customer' => false,
          'supplier' => true,
        )
      );
      $this->ref['res.partner'][$rec['Supplier']] = $sup_id;
      unset($q);
    }
  }

  public function submit($form, &$form_state)
  {
    $recs = $this->getRecs($form);
    $ops = array();

    $Fac = new \Oerp\Query\Operation\Factory();

    $arch = file_get_contents(__DIR__ . '/ProductData/product_op.xml');
    $ops = array_merge($ops, $Fac->getOperations($arch, $recs));

//    $arch = file_get_contents(__DIR__ . '/ProductData/pricelist_op.xml');
//    $supps = \Gulei\_Array::lambda($recs, 'promote', 'Supplier');
//    $ops = array_merge($ops, $Fac->getOperations($arch, $supps));

    foreach($ops as $op) {
      $op->run();
    }

    drupal_set_message(sprintf('%d item(s) imported.', count($recs)));

    $cmd = new \Oerp\Product\Command\assignSkuSeq();
    $cmd->execute();
  }
}