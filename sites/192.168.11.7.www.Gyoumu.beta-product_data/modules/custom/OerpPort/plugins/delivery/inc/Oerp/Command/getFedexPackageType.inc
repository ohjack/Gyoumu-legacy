<?php
namespace Oerp\Command;

class getFedexPackageType extends \Gulei\Command\WithReference\WithTable
{
  function execute($refVar = null)
  {
    static $mapping;

    list($rec, $name) = $this->ref->vals('rec', 'name');

    if(!$mapping){
      $csvPkg = new \Gulei\Csv\Model(array(
        'filepath' => drupal_get_path('module', 'oerp_delivery') . '/FedexPackaging.csv',
      ));

      $ArryMapping = new \Gulei\_Array\Basic($csvPkg->connect());
      $mapping = (array)$ArryMapping->promote('SKU');
    }

    $skus = explode("\n", $rec["move_lines.product_id.default_code"]);
    $qtys = explode("\n", $rec["move_lines.product_qty"]);

    if(count($skus) > 1 || $qtys[0] > 1 || !isset($mapping[$skus[0]])){
      if($name == 'func:pkg_type'){
        return '01';
      }
      else if($name == 'func:adm_pkg_type'){
        return 'PCS';
      }
    }
    else{
      $pkg_map = array(
        'Fedex Box' => array('03','BOX'),
        'Fedex Tube' => array('04', 'TBE'),
      );

      $type = $mapping[$skus[0]]['packaging'];

      if($name == 'func:pkg_type'){
        $pack = $pkg_map[$type][0];
      }
      else if($name == 'func:adm_pkg_type'){
        $pack = $pkg_map[$type][1];
      }

      if(!$pack){
        throw new Exception('getFedexPackageType: Unknown packaging type.');
      }
      return $pack;
    }
  }
}