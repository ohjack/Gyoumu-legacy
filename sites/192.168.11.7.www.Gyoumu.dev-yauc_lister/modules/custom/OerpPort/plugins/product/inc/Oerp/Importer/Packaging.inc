<?php
namespace Oerp\Importer;

class Packaging extends \Gulei\Importer\Arch\_Abstract
{
  protected function getClassname() {
    return __CLASS__;
  }

  public function getArch() {
    return file_get_contents(__DIR__ . '/Packaging.xml');
  }

  public function submit($form, &$form_state) {
    $recs = $this->getBufferObj($form)->getRecordsAltered();
    $q = new \Oerp\Query\Basic('product.product', NULL, array('default_code'));
    $prods = (array) $q->raw()->promote('default_code');

    foreach ($recs as $ind => &$rec) {
      foreach (array('L', 'W', 'H') as $fld) {
        if (!$rec[$fld]) {
          $rec[$fld] = '0';
        }
      }

      if (isset($prods[$rec['SKU']])) {
        $rec['product_id'] = $prods[$rec['SKU']]['id'];
      }
      else {
        unset($recs[$ind]);
      }
    }

    $Fac = new \Oerp\Query\Operation\Factory();

    $arch = file_get_contents(__DIR__ . '/Packaging/packaging_op.xml');
    $ops = $Fac->getOperations($arch, $recs);

    foreach ($ops as $op) {
      $op->run();
    }
    drupal_set_message('Packagings imported.');
  }

  public function clear() {
    $q = new \Oerp\Query\Basic('product.packaging');
    $pkgs = $q->raw();

    foreach ($pkgs as $pkg) {
      $q = new \Oerp\Query\Basic('product.packaging', NULL, NULL, $pkg['id']);
      $q->unlink();
    }
  }
}