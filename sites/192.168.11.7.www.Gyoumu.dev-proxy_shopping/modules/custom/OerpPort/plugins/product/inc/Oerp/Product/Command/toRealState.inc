<?php
namespace Oerp\Product\Command;

class toRealState extends \Gulei\Command\WithReference\WithTable
{
  public function execute($refVar = null)
  {
    list($rec) = $this->ref->vals('rec');
    $root = $this->getRootName();

    switch($rec[$root . 'state']){
      case 'draft':
      case 'In Development':
        return '開發中';

      case 'end':
      case 'End of Lifecycle':
        return 'EoL';

      case 'sellable':
      case 'In Production':
        $pur_ok = $rec[$root . 'purchase_ok'];

        if ((is_bool($pur_ok) && $pur_ok === FALSE)
            || (is_string($pur_ok) && $pur_ok === 'No')) {

          return 'Close';
        }
        return 'Ready';
    }
    return false;
  }
}