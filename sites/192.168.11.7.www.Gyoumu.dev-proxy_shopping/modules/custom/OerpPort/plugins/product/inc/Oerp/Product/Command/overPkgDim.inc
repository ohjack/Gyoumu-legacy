<?php
namespace Oerp\Product\Command;

class overPkgDim extends \Gulei\Command\WithReference\WithTable
{
  public function execute($refVar = null)
  {
    static $refDim = null;
    if(is_null($refDim)){
      $q = new \Oerp\Query\Basic('product.ul');
      $refDim = $q->raw()->promote('id');
    }

    list($rec, $name) = $this->ref->vals('rec', 'name');
    $pkg_id = $rec['packaging.ul.id'];

    if (!$pkg_id) {
      return FALSE;
    }

    $pkg = $refDim[$pkg_id];
    $toks = explode('.', $name);
    $fld = array_pop($toks);

    if(($val = $pkg['x_' . $fld]) > 0){
      return sprintf('<span style="color: #ff4500;">%s</span>', $val);
    }
    return false;
  }
}