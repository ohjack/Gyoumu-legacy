<?php
namespace Oerp\Importer;

class Packaging extends \Gulei\Importer\Arch\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function getArch()
  {
    return file_get_contents(__DIR__ . '/Packaging.xml');
  }

  public function submit($form, &$form_state)
  {
    $recs = $this->getBufferObj($form)->getRecords();
    $q = new \Oerp\Query\Basic('product.product', null, array('default_code'));
    $prods = (array)$q->raw()->promote('default_code');

    foreach($recs as $ind => &$rec){
      if(!$rec['SKU'] || !($rec['L'] || $rec['W'] || $rec['H'])){
        unset($recs[$ind]);
      }

      if(isset($prods[$rec['SKU']])){
        $rec['product_id'] = $prods[$rec['SKU']]['id'];
      }
      else{
        unset($recs[$ind]);
      }
    }

    $Fac = new \Oerp\Query\Operation\Factory();

    $arch = file_get_contents(__DIR__ . '/Packaging/packaging_op.xml');
    $ops = $Fac->getOperations($arch, $recs);

    foreach($ops as $op){
      $op->run();
    }
  }

  public function clear()
  {
    $q = new \Oerp\Query\Basic('product.packaging');
    $pkgs = $q->raw();

    foreach($pkgs as $pkg){
      $q = new \Oerp\Query\Basic('product.packaging', null, null, $pkg['id']);
      $q->unlink();
    }
  }
}